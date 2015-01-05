#include"CMD_Protocol.h"
#define MAX_CMD_PRO_LEN 6
int cmd_socket;

int cleancmdbuff(char *s){
    int  i;
    for(i=0;i<MAX_CMD_PRO_LEN;i++){
        s[i]= '\0';
    }
    return True;
}

int proto_init_cmd_server_for_rc(int port){
        // tell the rssocks a new connect
    struct sockaddr_in listen_server,client;
    int socks_server,client_sock;
    int len_sockaddr;
    puts("init cmd_server_for_rc here");
    socks_server = API_socket_init_server(port,300);
    len_sockaddr = sizeof(struct sockaddr_in);
    puts("start listen port here");
    while( (client_sock = accept(
                socks_server,
                (struct sockaddr *)&client,
                (socklen_t*)&len_sockaddr
                ))>= 0 ){
    // puts("new clent from second server here");
        proto_understand_and_do_it( client_sock );
        usleep(1);
    }
    return PROTO_INIT_CMD_SER_OK;
}

int proto_init_cmd_rcsocket(char *URL,int port){
    int cmd_sock; int readsize,sendsize;
    char cmd[MAX_CMD_PRO_LEN];
    cmd_sock = API_socket_connect(URL,port);
    if(cmd_sock == SOCKET_CONNECT_ERROR){
        printf("Error on connect %s:%d [proto_init_cmd_rcsocket]",URL,port);
        return PROTO_INIT_CMD_RCSOCKET_ERROR;
    }
    // ready send New_rcsocks_Reconnect
    cleancmdbuff(cmd);
    cmd[0] = PRO_MAKE_NET;
    cmd[1] = I_AM_NEW_RC_CMD_SOCK_CLIENT;
    sendsize = API_socket_send(cmd_sock,cmd,MAX_CMD_PRO_LEN);
    // check send OK ?
    if(sendsize != MAX_CMD_PRO_LEN ){
        printf("Error on send I_AM_NEW_RC_CMD_SOCK_CLIENT.\n");
        return PROTO_INIT_CMD_RCSOCKET_ERROR;
    }
    // check server create OK ?
    readsize = API_socket_recv(cmd_sock,cmd,MAX_CMD_PRO_LEN);
    if(readsize != MAX_CMD_PRO_LEN){
        printf("Error on recv CONFIRM_YOU_ARE_SOCK_CLIENT 1.\n");
        return PROTO_INIT_CMD_RCSOCKET_ERROR;
    }
    if(cmd[0]!= PRO_MAKE_NET &&
         cmd[1]!=CONFIRM_YOU_ARE_SOCK_CLIENT ){
        printf("Error on recv CONFIRM_YOU_ARE_SOCK_CLIENT 2.\n");
        return PROTO_INIT_CMD_RCSOCKET_ERROR;
    }
    return cmd_sock;
}

int proto_get_rcsocket(char *URL,int port,int poolnum){
    int rcsock;int readsize,sendsize;
    char cmd[MAX_CMD_PRO_LEN];char buf[4];
    rcsock = API_socket_connect(URL,port);
    cmd[0] = PRO_MAKE_NET;
    cmd[1] = I_AM_NEW_RC_SOCK_FOR_TUNNEL;
    API_m_itochar(poolnum,buf,4);
    cmd[2] = buf[0];
    cmd[3] = buf[1];
    cmd[4] = buf[2];
    cmd[5] = buf[3];
    sendsize = API_socket_send(rcsock,cmd,MAX_CMD_PRO_LEN);
    if(sendsize != MAX_CMD_PRO_LEN){
        printf("Error on send new tunnel cmd\n");
        return PROTO_GET_RCSOCKET_ERROR;
    }
    //printf("create a new rc socket\n");
    readsize = API_socket_recv(rcsock,cmd,MAX_CMD_PRO_LEN);
    if(readsize != MAX_CMD_PRO_LEN){
        printf("Error on recv CONFIRM_YOU_ARE_SOCK_TUNNEL\n");
        return PROTO_GET_RCSOCKET_ERROR;
    }
    if(cmd[0] != PRO_MAKE_NET ||
         cmd[1] != CONFIRM_YOU_ARE_SOCK_TUNNEL){
        printf("Error on recv CONFIRM_YOU_ARE_SOCK_TUNNEL 2\n");
        return PROTO_GET_RCSOCKET_ERROR;
         
    }
    return rcsock; 
}
    
int proto_understand_and_do_it(int sock){
    int readSize=-1;
    char cmd[MAX_CMD_PRO_LEN];

    readSize = API_socket_recv(sock,cmd,MAX_CMD_PRO_LEN);
    if(readSize == MAX_CMD_PRO_LEN && cmd[0] == PRO_MAKE_NET){
        // create network pro
        if( cmd[1] == I_AM_NEW_RC_CMD_SOCK_CLIENT ){
            cleancmdbuff(cmd);
            cmd[0] = PRO_MAKE_NET;
            cmd[1] = CONFIRM_YOU_ARE_SOCK_CLIENT;
            API_socket_send(sock,cmd,MAX_CMD_PRO_LEN);
            puts("rssocks cmd_socket OK!");
        }
        else if (cmd[1] == I_AM_NEW_RC_SOCK_FOR_TUNNEL){
            int num = API_m_chartoi(&(cmd[2]),4);
            cleancmdbuff(cmd);
            cmd[0] = PRO_MAKE_NET;
            cmd[1] = CONFIRM_YOU_ARE_SOCK_TUNNEL;
            API_socket_send(sock,cmd,MAX_CMD_PRO_LEN);
            tunn_set_second_pool_and_run_it(num,sock);
        }
        else return PROTO_UNDERSTAND_ERROR;
        return proto_UNDERSTAND_AND_DO_OK;
    }
    return PROTO_UNDERSTAND_ERROR;
}

int proto_send_rccmd_poolnum(int sock,int poolnum){
    char cmd[MAX_CMD_PRO_LEN];
    char buf[4];
    cleancmdbuff(cmd);
    cmd[0] = PRO_MAKE_NET;
    cmd[1] = START_A_NEW_SOCK_TUNNEL_NOW;
    API_m_itochar(poolnum,buf,4);
    cmd[2] = buf[0];
    cmd[3] = buf[1];
    cmd[4] = buf[2];
    cmd[5] = buf[3];
    if(API_socket_send(sock,cmd,MAX_CMD_PRO_LEN) != MAX_CMD_PRO_LEN){
        return PRO_SEND_RCCMD_ERROR;
    }
    return PRO_SEND_RCCMD_OK;
}
