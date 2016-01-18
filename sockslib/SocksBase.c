#include "SocksBase.h"

int socks_build_target_socket(int sock){
    char buffer[2000],buf[200];
    int read_size , write_size ;
    int mode , addrtype ;
    int cli_sock;
    int url_len,i;
    int des_port;
    unsigned int addrvalue;
    char addrURL[300];
    struct in_addr buf_addr;
    char reply[200];
    int reply_len = -1;
    struct hostend * des_host;
    read_size = write_size = 0;
    if (sock <= 0) return SOCKS_BUILD_TARGET_SOCKET_ERROR;
    // 1. Version
    read_size = API_socket_recv(sock , buffer, 262 );
    if (read_size > 262 || read_size < 0 ){ 
        return SOCKS_BUILD_TARGET_SOCKET_ERROR ;
    }
    buf[0] = 0x05 ; buf[1] = 0x00;
    write_size = API_socket_send( sock ,buf , 2 ) ;
    if ( write_size != 2 ) {
        return SOCKS_BUILD_TARGET_SOCKET_ERROR;
    }
    // 2. Request
    read_size = API_socket_recv ( sock , buffer, 4 );
    if (read_size != 4){ 
        return SOCKS_BUILD_TARGET_SOCKET_ERROR ; 
    }
    mode = buffer[1];
    addrtype = buffer[3];
    // 2.1 read url
    if(addrtype == 1){ // IPv4
        read_size = API_socket_recv(sock , (char *)&addrvalue , 4 );
        buf_addr.s_addr = addrvalue;
        strcpy(addrURL, inet_ntoa(buf_addr));
        printf("the recv ip is %s",addrURL);
    }
    else if(addrtype == 3){
        read_size = API_socket_recv(sock ,buf , 1 );
        if (read_size != 1){
            return SOCKS_BUILD_TARGET_SOCKET_ERROR; 
        }
        url_len = buf[0];
      //printf("3\n read_size = %d -- buf[0] = %d ",read_size,buf[0]);
        if ( url_len > 256 || url_len <= 0 ){ return -1; }
        read_size = API_socket_recv(sock ,addrURL , url_len);
        if (read_size != url_len ) {
            addrURL[read_size] = '\0';
            printf("Something error on read URL\n");
            printf("the read url is %s \n",addrURL);
            return -1;
        }
        addrURL[read_size] = '\0';
    }
    // 2.2 read port
    read_size = API_socket_recv ( sock ,buf ,2 );
    if (read_size != 2 ){
        return SOCKS_BUILD_TARGET_SOCKET_ERROR ;
    }
    des_port =(unsigned char ) buf[0]*256 + (unsigned char )buf[1];
    reply[0] = 0x05; reply[1] = 0x00; reply[2] = 0x00; reply[3] = 0x01;
    reply_len = 4;
    // 2.3 connect URL : port
    if (mode == 1){ // tcp
        printf (" Tcp ---> %s:%d \n",addrURL,des_port);
        cli_sock = API_socket_connect(addrURL,des_port);
    }
    else {
        reply[0] = 0x05;reply[1] = 0x07 ;reply[2]=0x00;reply[3]=0x01;
        reply_len = 4;
        return SOCKS_BUILD_TARGET_SOCKET_ERROR;
    }
    for (i = 0;i<6;i++){
        reply[reply_len] = 0x41;
        reply_len ++;
    }
    API_socket_send(sock,reply, reply_len);
    //puts("sock5pro return cli_sock\n");
    return cli_sock; 
}

MIC_THREAD_FUN_DEF(socks_check_and_tunnel,sock){
////void *socks_check_and_tunnel(void *sock){
//    int new_sock,sendsize,readsize;
//    int target_sock;
//    char cmd_buff[100];
//    struct rcsocktul *tunn = (struct rcsocktul *)sock;
//    if(tunn == NULL){
//        MIC_THREAD_END();
////        pthread_detach(pthread_self());
//        return NULL;
//    }
//    //printf("create RC_rcosket\n");
//    new_sock = proto_get_rcsocket(tunn->URL,tunn->port,tunn->poolnum);
//    if(new_sock == PROTO_GET_RCSOCKET_ERROR){
//        API_socket_close(new_sock);
//        return NULL;
//    }
//    target_sock = socks_build_target_socket(new_sock);
//    if( target_sock != SOCKS_BUILD_TARGET_SOCKET_ERROR){
//        tunn_sock_to_sock(target_sock,new_sock,API_get_usec_time());
//    }
//    else{
//        API_socket_close(new_sock);
//    }
//    MIC_THREAD_END();
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

