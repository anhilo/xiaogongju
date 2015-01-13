#include"Lcx_Base.h"

char from_URL[300],to_URL[300];
int from_port,to_port;

MIC_THREAD_FUN_DEF(create_socks_port_server,p_port) {
//void *create_socks_port_server(void *p_port){
    // get connect from explorer
    int port = *(int*)p_port;
    int socks_server,client_sock;
    struct sockaddr_in client;
    int len_sockaddr,poolnum;
    MIC_THREAD_HANDLE_ID thread_id;
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
            API_get_usec_time()
        );
        proto_send_rccmd_poolnum(poolnum);
        MIC_USLEEP(1);
    }
    if(client_sock<0){
        puts("accept failed");
        return 0;
    }
    MIC_THREAD_END();
//    pthread_detach(pthread_self());
    puts("exit socks_port_server");
    return 0;
}


MIC_THREAD_FUN_DEF(create_listen_port,p_port) {
//void * create_listen_port(void *p_port){
    // tell the rssocks a new connect
    int port = *(int *)p_port;
    proto_init_cmd_server_for_rc(port);
    MIC_THREAD_END();
    return NULL;
}

int lcx_listen(int from_port,int cmd_port,int usec){
    // rcsocks -l socks_port -p listen_port
    MIC_THREAD_HANDLE_ID thread_1 ,thread_2;
    API_set_usec_time(usec);
    printf("rcsocks 0.0.0.0:%d <--[%4d usec]--> 0.0.0.0:%d\n",
    from_port, API_get_usec_time() , cmd_port );
    if(MIC_THREAD_CREATE( thread_1,
        create_socks_port_server, &from_port) < 0)
//    if(pthread_create(&thread_1,NULL,
//    create_socks_port_server,&from_port)<0)
    {
        puts("could not create one way tunnel\n");
        return -1;
    }
    //puts("create first server OK!");
    if(MIC_THREAD_CREATE( thread_1,
        create_listen_port, &cmd_port) < 0)
//    if(pthread_create(&thread_2,NULL,
//    create_listen_port,&cmd_port)<0)
    {
        puts("could not create one way tunnel\n");
        return -1;
    }
//    pthread_join(thread_1,NULL);
//    pthread_join(thread_2,NULL);
    while(1){
        MIC_SLEEP(1000);
    }
    puts("exit rcsocks now ????");
    return 1; 
}



int lcx_tran(int from_port,char *to_host,int to_port,int usec){
//    strncpy(to_URL ,mto_host,300);
//    to_port = mto_port;
//    usec_time = usec;
    int server,client_sock;
    struct sockaddr_in client;
    int len_sockaddr = sizeof(struct sockaddr_in);
    int target_sock;
    API_set_usec_time(usec);
    printf("lcx_tran 0.0.0.0:%d <--[%4d usec]--> %s:%d\n",
        from_port, API_get_usec_time() , to_host,to_port);
    server = API_socket_init_server(from_port,300);
    while( ( client_sock = accept(server,
      (struct sockaddr *)&client,
      (socklen_t*)&len_sockaddr)
      ) > 0){
        target_sock = API_socket_connect(to_host,to_port);
        //printf("get target OK ????\n");
        if(target_sock != SOCKET_INIT_ERROR &&
          client_sock != SOCKET_INIT_ERROR ){
            tunn_sock_to_sock(target_sock,
            client_sock,
            API_get_usec_time());
        }
        else{
            API_socket_close(target_sock);
            API_socket_close(client_sock);
        }
    }
    printf("close tran ????\n");
    return 1;
}

MIC_THREAD_FUN_DEF(check_and_slave_tunnel,PoolNum) {
//void *check_and_slave_tunnel(void *PoolNum){
    //puts("check slave now !!");
    int target_sock,new_sock;
    if(PoolNum == NULL){
        MIC_THREAD_END();
//        pthread_detach(pthread_self());
        return NULL;
    }
    int poolnum = *(int*)PoolNum;
    //printf("poolnum = %d \n",poolnum);
    new_sock = proto_get_rcsocket(from_URL,from_port,poolnum);
    target_sock = API_socket_connect(to_URL,to_port);
    if(new_sock != SOCKET_INIT_ERROR &&
      target_sock != SOCKET_INIT_ERROR){
        tunn_sock_to_sock(new_sock,target_sock,API_get_usec_time());
    }
    else{
        API_socket_close( new_sock );
        API_socket_close( target_sock ) ;
    }
    MIC_THREAD_END();
//    pthread_detach(pthread_self());
    return NULL;
}

int lcx_slave(char *mfrom_URL,int mfrom_port,
        char *mto_URL,int mto_port,int usec){
    int readsize,sendsize;
    char cmd_buff[MAX_CMD_PRO_LEN];
    int poolnum;int cmd_sock;
    API_set_usec_time(usec);
    strncpy(from_URL,mfrom_URL,300);
    strncpy(to_URL,mto_URL,300);
    from_port = mfrom_port;
    to_port   = mto_port;
    printf("lcx_slave %s:%d <--[%4d usec]--> %s:%d\n",
        from_URL,from_port, usec, to_URL,to_port);
    cmd_sock = proto_init_cmd_rcsocket(from_URL,from_port);
    if(cmd_sock == SOCKET_INIT_ERROR ){return -1;}
    while(1){
        readsize = API_socket_recv(cmd_sock,cmd_buff,MAX_CMD_PRO_LEN);
        //printf("readsize = %d \n",readsize);
        if( readsize == MAX_CMD_PRO_LEN &&
          cmd_buff[0]== PRO_MAKE_NET && 
          cmd_buff[1]== START_A_NEW_SOCK_TUNNEL_NOW ){
            // read a pool number
            MIC_THREAD_HANDLE_ID thread_id;
            poolnum = API_m_chartoi( &(cmd_buff[2]),4 );
            if(MIC_THREAD_CREATE(thread_id,
                check_and_slave_tunnel,
                &poolnum) < 0 ) {
//            if(pthread_create(&thread_id,NULL,
//              check_and_slave_tunnel,
//              (void *)&poolnum)<0) {
                puts("could not create one way tunnel\n");
                return 0;
            }
            MIC_USLEEP(1);
        }
        else{
            lcx_slave(mfrom_URL,mfrom_port,mto_URL,mto_port,usec);
            return 1;
        }
        MIC_USLEEP(1);
    } 
}
