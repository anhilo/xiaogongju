#include "ssocksd_pro.h"

MIC_THREAD_FUN_DEF(check_and_tunnel,socket_desc) {
//void *check_and_tunnel(void *socket_desc) {
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size,write_size;
    char *message , client_message[2000];
    int client_sock;
    //Receive a message from client
    //read_size = recv(sock , client_message , 2000 , 0);
    MIC_USLEEP(1);
    client_sock = socks_build_target_socket(sock);
    //client_sock = get_connect_socket("127.0.0.1",8090);
    if ( client_sock != SOCKS_BUILD_TARGET_SOCKET_ERROR ){
        tunn_sock_to_sock(client_sock,sock, API_get_usec_time());
    }
    else{
        //printf("client_sock == -1\n");
        API_socket_close(sock);
    }
    //printf("close check_and tunnel now ??socket= %d??\n",sock);
    MIC_THREAD_END();
//  pthread_detach(pthread_self());
    // mclose(sock);
    return NULL;
}



int create_ssocksd_server(int port,int usec){
    int socket_desc , client_sock , c;
    int sockbuf;
    struct sockaddr_in server , client;
    API_set_usec_time(usec);
    socket_desc = API_socket_init_server(port,300);
    c = sizeof(struct sockaddr_in);
    printf("ssocksd 0.0.0.0:%d <--[%4d usec]--> socks server\n",
    port , API_get_usec_time());
    //Accept and incoming connection
    // puts("Waiting for incoming connections...");
    MIC_THREAD_HANDLE_ID thread_id;
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c))>=0 )
    {
        // puts("Connection accepted");
        sockbuf = client_sock;
        //printf("new check_and_tunnel now socket=%d\n",sockbuf);
        if( MIC_THREAD_CREATE ( thread_id,check_and_tunnel,
            &sockbuf) < 0 ) {
//        if( pthread_create( &thread_id , NULL , check_and_tunnel , (void*) &sockbuf) < 0) {
            API_socket_close(sockbuf);
            puts("could not create thread");
            return CREATE_SSOCKSD_SERVER_ERROR;
        }
        client_sock = -1;
        MIC_USLEEP(1);
//#ifndef __APPLE__
//        MIC_THREAD_JOIN( thread_id );
//#endif
        //Now join the thread , so that we dont terminate before the thread
        // pthread_join( thread_id , NULL);
        // puts("Handler assigned");
    }
    if (client_sock < 0)
    {
        puts("accept failed");
        return CREATE_SSOCKSD_SERVER_ERROR;
    }
    printf("ssocksd close ?????\n");
    return CREATE_SSOCKSD_SERVER_OK;
}
