#include "Sock_Tunnel.h"

#define NULL_SOCK         0
#define SOCK_1_OK         1
#define SOCK_2_OK         2
#define POOL_WORKING_NOW  3
#define POOL_STOP_NOW     4

sockbind_s socks_Pool[MAX_POOL];
int live_num = 0;
int can_write_pool = True;

int tunn_init_Pool(){
   int i;
    for(i = 0;i<MAX_POOL;i++){
        socks_Pool[i].sock_1 = -1;
        socks_Pool[i].sock_2 = -1;
        socks_Pool[i].flag   = False;
        socks_Pool[i].state  = NULL_SOCK;
        socks_Pool[i].usec_time = -1;
    } 
    can_write_pool = True;
    return TUNNEL_INIT_OK;
}

int tunn_clean(int num){
    int i = num;
    socks_Pool[i].sock_1 = -1;
    socks_Pool[i].sock_2 = -1;
    socks_Pool[i].flag = False;
    socks_Pool[i].state = NULL_SOCK;
    socks_Pool[i].usec_time = -1;
    live_num --;
    //can_write_pool = True;
    return TUNNEL_CLEAN_OK;
}
int tunn_close(int num){
    if( socks_Pool[ num ].sock_1 > 0){
        API_socket_close(socks_Pool[ num ].sock_1);
        socks_Pool[ num ].sock_1 = -1;
    }
    if( socks_Pool[ num ].sock_2 > 0){
        API_socket_close(socks_Pool[ num ].sock_2);
        socks_Pool[ num ].sock_2 = -1;
    }
   //puts("close tunnel");
    socks_Pool[ num ].usec_time = -1;
    socks_Pool[ num ].state = NULL_SOCK;
    socks_Pool[ num ].flag = False;
    live_num --;
//    printf("--> %3d <-- (close)used tunnel %d , unused tunnel %d\n",num,live_num,MAX_POOL - live_num);
    return 1; 
}

void * tunn_run_now(void *p_num){
    int num;
    int read_size , write_size;
    char buffer[TUNMAX_BUF];
    int times = 0;
    // data must be here, so do a normal recv
    int n1=1,n2=1;
    num = *(int*)p_num;
    int sock1,sock2;
    sock1 = socks_Pool[ num ].sock_1;
    sock2 = socks_Pool[ num ].sock_2;
    //printf("sock1 = %d , sock2 = %d\n",sock1,sock2);
//    printf("--> %3d <-- (start)used tunnel %d , unused tunnel %d\n",num,live_num,MAX_POOL - live_num);
    while(times < socks_Pool[ num ].usec_time && socks_Pool[num].usec_time>=0){
        n1 = API_socket_read_state(sock1,0,1);
        n2 = API_socket_read_state(sock2,0,1);
        if (n1 == SOCKET_INIT_ERROR || n2 == SOCKET_INIT_ERROR ){ // Sock_Error
        //printf("n1 n2 <0 ???\n");
            break;
        }
        if(n1 == SOCKET_CAN_READ_STATE
        && (read_size = API_socket_recv(sock1,buffer,TUNMAX_BUF)) > 0){
            times = 0;
            write_size = API_socket_send( sock2 ,buffer,read_size);
            if( write_size < 0 || write_size != read_size){
            // socks_Pool[num].sock_2 = -1;
                break;
            }
        }
        else if( n1 == SOCKET_MAY_CLOSE_NOW ||
        (n1==SOCKET_CAN_READ_STATE && read_size <= 0) ){
        //socks_Pool[num].sock_1 = -1;
        // puts("close other side n1 >0");
            break;
        } // 对方关闭了 sock会话 }
        if(n2 == SOCKET_CAN_READ_STATE
        && (read_size = API_socket_recv(sock2,buffer,TUNMAX_BUF)) > 0){
            times = 0;
            write_size = API_socket_send( sock1 ,buffer,read_size );
            if( write_size < 0 || write_size != read_size){
            //socks_Pool[num].sock_1 = -1;
                break;
            }
        }
        else if( n2 == SOCKET_MAY_CLOSE_NOW ||
        (n2==SOCKET_CAN_READ_STATE && read_size <= 0) ){
        //socks_Pool[num].sock_2 = -1;
        // puts("close other side n2>0");
            break;
        } // 对方关闭了 sock会话 }
        memset(buffer , 0 , TUNMAX_BUF);
        read_size = write_size = 0;
        n1 = n2 = -1;
        usleep(1);
        times = times +1;
    }
    pthread_detach(pthread_self());
    tunn_close( num );
    //printf("close tunnel_now !!!\n");
    return 0; 
}

int tunn_get_pool_id_and_lock_it(){
    int i=0;
    while(live_num >= MAX_POOL-1 || !can_write_pool){
    // wait Pool ready
        usleep(1);
    }
    // lock pool write
    can_write_pool = False;
    // find a Null Pool
    for (i = 0;i<MAX_POOL;i++){
        if(socks_Pool[i].flag == False){
            live_num ++;
            socks_Pool[i].flag = True;
            return i;
        }
    }
    return TUNNEL_GET_POOL_ID_FALSE;
}

int tunn_set_first_pool_and_lock_it(int sock,int usec){
    int id ;
    id = tunn_get_pool_id_and_lock_it();
    if(id != TUNNEL_GET_POOL_ID_FALSE){
        socks_Pool [ id ].flag      = True; // replace this pool
        socks_Pool [ id ].sock_1    = sock;
        socks_Pool [ id ].state     = SOCK_1_OK;
        socks_Pool [ id ].usec_time = usec;
    }
    else {
        id = TUNNEL_GET_FIRST_POOL_ID_FALSE;
    }
    // unlock write pool now
    can_write_pool = True;
    return id;   
}

int tunn_set_second_pool_and_run_it(int num,int sock){
    int *poolnum= (int *)malloc(sizeof(int));
    *poolnum = num;
    pthread_t thread_1;
    socks_Pool[ num ] .sock_2 = sock;
    socks_Pool[ num ] .state = SOCK_2_OK;
    if( pthread_create( &thread_1,NULL ,
      tunn_run_now,
      (void *)poolnum)<0){
        perror("could not create one way tunnel\n");
        tunn_close(num);
        return TUNNEL_SET_SECOND_POOL_AND_RUN_IT_FALSE;
    }
    // pthread_join(thread_1,NULL);
    return TUNNEL_SET_SECOND_POOL_AND_RUN_IT_OK;
}

int tunn_sock_to_sock(int from_sock,int to_sock,int usec){
    int num = tunn_set_first_pool_and_lock_it(from_sock,usec);
    if (num != TUNNEL_GET_FIRST_POOL_ID_FALSE){
        if(TUNNEL_SET_SECOND_POOL_AND_RUN_IT_FALSE !=
             tunn_set_second_pool_and_run_it(num,to_sock)){
            return TUNNEL_SOCK_TO_SOCK_OK;
        }
    }
    return TUNNEL_SOCK_TO_SOCK_ERROR;
}
