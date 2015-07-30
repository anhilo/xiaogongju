#include "SocksBase.h"
#include "libsock/Socks.h"

int socks_build_target_socket(int sock){
    int cli_sock;
    cli_sock = Build_Target_Socket(sock);
    if(cli_sock == BUILD_TARGET_SOCKET_ERROR){
        return SOCKS_BUILD_TARGET_SOCKET_ERROR;
    }
    return cli_sock; 
}

MIC_THREAD_FUN_DEF(socks_check_and_tunnel,sock){
//void *socks_check_and_tunnel(void *sock){
    int new_sock,sendsize,readsize;
    int target_sock;
    char cmd_buff[100];
    struct rcsocktul *tunn = (struct rcsocktul *)sock;
    if(tunn == NULL){
        MIC_THREAD_END();
//        pthread_detach(pthread_self());
        return NULL;
    }
    //printf("create RC_rcosket\n");
    new_sock = proto_get_rcsocket(tunn->URL,tunn->port,tunn->poolnum);
    if(new_sock == PROTO_GET_RCSOCKET_ERROR){
        API_socket_close(new_sock);
        return NULL;
    }
    target_sock = socks_build_target_socket(new_sock);
    if( target_sock != SOCKS_BUILD_TARGET_SOCKET_ERROR){
        tunn_sock_to_sock(target_sock,new_sock,API_get_usec_time());
    }
    else{
        API_socket_close(new_sock);
    }
    MIC_THREAD_END();
//    pthread_detach(pthread_self());
    return NULL;
}



int socks_build_rcsocks_tunnel(void *sock){
    MIC_THREAD_HANDLE_ID thread_id;
    if ( MIC_THREAD_CREATE ( thread_id , 
        socks_check_and_tunnel,
        sock) < 0 ) {
//            if(pthread_create(&thread_id,NULL,
//            socks_check_and_tunnel,(void *)tunn)<0) {
        puts("could not create one way tunnel\n");
        return 0;
    }
    return 1;
}

