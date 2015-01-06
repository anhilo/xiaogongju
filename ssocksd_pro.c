#include "ssocksd_pro.h"

void *check_and_tunnel(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size,write_size;
    char *message , client_message[2000];
    int client_sock;
    //Receive a message from client
    //read_size = recv(sock , client_message , 2000 , 0);
    client_sock = socks_build_target_socket(sock);
    //client_sock = get_connect_socket("127.0.0.1",8090);
    if ( client_sock != SOCKS_BUILD_TARGET_SOCKET_ERROR )
        tunn_sock_to_sock(client_sock,sock, API_get_usec_time());
    else{
        //printf("client_sock == -1\n");
        API_socket_close(sock);
    }
    //printf("close check_and tunnel now ??socket= %d??\n",sock);
    pthread_detach(pthread_self());
    // mclose(sock);
    return NULL;
}



int create_ssocksd_server(int port,int usec){
    int socket_desc , client_sock , c;
    struct sockaddr_in server , client;
    API_set_usec_time(usec);
    socket_desc = API_socket_init_server(port,300);
    c = sizeof(struct sockaddr_in);
    printf("ssocksd 0.0.0.0:%d <--[%4d usec]--> socks server\n",
    port , API_get_usec_time());
    //Accept and incoming connection
    // puts("Waiting for incoming connections...");
    pthread_t thread_id;
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c))>=0 )
    {
        // puts("Connection accepted");
        int sockbuf;
        sockbuf = client_sock;
        //printf("new check_and_tunnel now socket=%d\n",sockbuf);
        if( pthread_create( &thread_id , NULL , check_and_tunnel , (void*) &sockbuf) < 0)
        {
            API_socket_close(sockbuf);
            puts("could not create thread");
            return CREATE_SSOCKSD_SERVER_ERROR;
        }
        client_sock = -1;
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
