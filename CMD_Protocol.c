#include"CMD_Protocol.h"
#define MAX_CMD_PRO_LEN    6
    #define CMD_HEAD_LEN   2
    #define CMD_DATA_LEN   4
    //  MAX_CMD_PRO_LEN =
    //  CMD_HEAD_LEN + CMD_DATA_LEN

//协议偏移
//  1. 命令头偏移
#define CMD_TYPE  0   
#define CMD_NAME  1
//  2. 数据偏移
#define CMD_DATA  2

int cmd_socket;
//***********************************************************
int proto_Make_Net_send(int socket,char *buf,int data_len){
    int send_len = -1;
    buf[CMD_TYPE] = PRO_MAKE_NET;
    send_len = API_socket_send(socket,buf,data_len+2);
    return send_len;
}

int proto_Check_Make_Net_CMD(char *buf){
    if( buf[CMD_TYPE] == PRO_MAKE_NET )
        return 1;
    else 
        return 0;
}

//***********************************************************
int proto_Check_CMD_NAME(char *buf,int flag){
    if( buf[CMD_NAME] == flag )
        return 1;
    else
        return 0;
}

int proto_Send_Make_Net_CMD_NAME(
        int sock,
        int flag,
        char *cmd,
        int data_len
    ){
    cmd[ CMD_NAME ] = flag;
    return proto_Make_Net_send( sock, cmd, data_len );
}


int proto_Fill_Data_With_poolnum(char *cmd, int poolnum){
    char buf[4];
    API_m_itochar(poolnum,buf,4);
    cmd[CMD_DATA+0] = buf[0];
    cmd[CMD_DATA+1] = buf[1];
    cmd[CMD_DATA+2] = buf[2];
    cmd[CMD_DATA+3] = buf[3];
    return 1;
}
//***********************************************************

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
        MIC_USLEEP(1);
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
//    sendsize = API_socket_send(cmd_sock,cmd,MAX_CMD_PRO_LEN);
    sendsize = proto_Send_Make_Net_CMD_NAME( cmd_sock,
        I_AM_NEW_RC_CMD_SOCK_CLIENT ,
        cmd , CMD_DATA_LEN );
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
//    cmd[0] = PRO_MAKE_NET;
//    cmd[1] = I_AM_NEW_RC_SOCK_FOR_TUNNEL;
//    API_m_itochar(poolnum,buf,4);
//    cmd[CMD_DATA+0] = buf[0];
//    cmd[CMD_DATA+1] = buf[1];
//    cmd[CMD_DATA+2] = buf[2];
//    cmd[CMD_DATA+3] = buf[3];
    proto_Fill_Data_With_poolnum( cmd, poolnum );
    sendsize = proto_Send_Make_Net_CMD_NAME(rcsock,
       I_AM_NEW_RC_SOCK_FOR_TUNNEL, 
       cmd, CMD_DATA_LEN );
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
//            cmd[0] = PRO_MAKE_NET;
//            cmd[1] = CONFIRM_YOU_ARE_SOCK_CLIENT;
            proto_Send_Make_Net_CMD_NAME( sock,
                CONFIRM_YOU_ARE_SOCK_CLIENT,
                cmd , CMD_DATA_LEN );
            cmd_socket  = sock;
            puts("rssocks cmd_socket OK!");
        }
        else if (cmd[1] == I_AM_NEW_RC_SOCK_FOR_TUNNEL){
            int num = API_m_chartoi(&(cmd[2]),4);
            cleancmdbuff(cmd);
//            cmd[0] = PRO_MAKE_NET;
//            cmd[1] = CONFIRM_YOU_ARE_SOCK_TUNNEL;
            proto_Send_Make_Net_CMD_NAME( sock,
                CONFIRM_YOU_ARE_SOCK_TUNNEL,
                cmd , CMD_DATA_LEN );
//            API_socket_send(sock,cmd,MAX_CMD_PRO_LEN);
            if( num == CHAR_TO_I_ERROR )
                return PROTO_UNDERSTAND_ERROR;
            tunn_set_second_pool_and_run_it(num,sock);
        }
        else return PROTO_UNDERSTAND_ERROR;
        return proto_UNDERSTAND_AND_DO_OK;
    }
    return PROTO_UNDERSTAND_ERROR;
}

int proto_send_rccmd_poolnum(int poolnum){
    char cmd[MAX_CMD_PRO_LEN];
    char buf[4];
    cleancmdbuff(cmd);
//    cmd[0] = PRO_MAKE_NET;
//    cmd[1] = START_A_NEW_SOCK_TUNNEL_NOW;
//    API_m_itochar(poolnum,buf,4);
//    cmd[CMD_DATA+0] = buf[0];
//    cmd[CMD_DATA+1] = buf[1];
//    cmd[CMD_DATA+2] = buf[2];
//    cmd[CMD_DATA+3] = buf[3];
    proto_Fill_Data_With_poolnum( cmd, poolnum );
    if(proto_Send_Make_Net_CMD_NAME( cmd_socket,
                START_A_NEW_SOCK_TUNNEL_NOW,
                cmd , CMD_DATA_LEN ) != 
        MAX_CMD_PRO_LEN){
        return PRO_SEND_RCCMD_ERROR;
    }
    return PRO_SEND_RCCMD_OK;
}
