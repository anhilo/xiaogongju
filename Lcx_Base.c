#include"Lcx_Base.h"
int lcx_usec_time = 1000;

void *create_socks_port_server(void *p_port){
    // get connect from explorer
    int port = *(int*)p_port;
    int socks_server,client_sock;
    struct sockaddr_in client;
    int len_sockaddr,poolnum;
    pthread_t thread_id;
    socks_server = API_socket_init_server(port,300);
    len_sockaddr = sizeof(struct sockaddr_in);
    while( (client_sock = accept(socks_server,
      (struct sockaddr *)&client,
      (socklen_t*)&len_sockaddr)
      ) >= 0){
        int sockbuf = client_sock;
        // puts("new clent from first server here");
        //update_pool_and_send_cmd(sockbuf);
        poolnum = tunn_set_first_pool_and_lock_it(
            sockbuf,
            lcx_usec_time
        );
        proto_send_rccmd_poolnum(poolnum);
        usleep(1);
    }
    if(client_sock<0){
        puts("accept failed");
        return 0;
    }
    pthread_detach(pthread_self());
    puts("exit socks_port_server");
    return 0;
}



void * create_listen_port(void *p_port){
    // tell the rssocks a new connect
    int port = *(int *)p_port;
    proto_init_cmd_server_for_rc(port);
    return NULL;
}

int lcx_listen(int from_port,int cmd_port,int usec){
    // rcsocks -l socks_port -p listen_port
    pthread_t thread_1 ,thread_2;
    lcx_usec_time = usec;
    printf("rcsocks 0.0.0.0:%d <--[%4d usec]--> 0.0.0.0:%d\n",
    from_port, usec , cmd_port );
    if(pthread_create(&thread_1,NULL,
    create_socks_port_server,&from_port)<0)
    {
        puts("could not create one way tunnel\n");
        return -1;
    }
    //puts("create first server OK!");
    if(pthread_create(&thread_2,NULL,
    create_listen_port,&cmd_port)<0)
    {
        puts("could not create one way tunnel\n");
        return -1;
    }
    pthread_join(thread_1,NULL);
    pthread_join(thread_2,NULL);
    puts("exit rcsocks now ????");
    return 1; 
}
