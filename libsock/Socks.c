#include "errno.h"
#include "Socks.h"

#define IPV4_TYPE 0x01
#define  URL_TYPE 0x03
#define IPV6_TYPE 0x04

#define TCP_TYPE  0x01

#define CONNECT_TARGET_OK        0x00  // 成功
#define CONNECT_TARGET_ERROR     0x01  // 一般性失败
#define NOT_SUPPORT_TUNNEL       0x02  // 规则不允许转发
#define NET_NOT_REACHABLE        0x03 // 网络不可达
#define THE_HOST_NOT_REACHABLE   0x04 // 主机不可达到
#define REFUSE_BY_REMOTE         0x05 // 连接拒绝
#define TTL_OVERTIME             0x06 // TTL 超时
#define NOT_SUPPORT_CMD          0x07 // 不支持请求包中的CMD
#define NOT_SUPPORT_ATYP         0x08 // 不支持请求包中的 ATYP
#define UNASSIGNED_TYPE          0xff // 不支持的标记
//#define CHECK_VERSION_OK           1

//#define CHECK_VERSION_ERROR        0

int Check_Version(int sock){
    char ERreq[2]={0x05,0xff};
    char OKreq[2] = {0x05,0x00};
    int  reqlen=2;
    char buffer[2000];
    int read_size,write_size;
    if(sock > 0){
        read_size = API_socket_recv(sock,buffer,262);
        if(read_size < 262 && read_size > 0){
            if(buffer[0] == SOCKS5_V){
                write_size = API_socket_send(sock,OKreq,reqlen);
                if (write_size == 2){
                    return CHECK_VERSION_OK;
                }
            }
            else {
printf ("SOCKSv4 Not Support now!");
            }
        }
    }
    write_size = API_socket_send(sock,ERreq,reqlen);
    return CHECK_VERSION_ERROR;
}

int Say_Cannot_Build_Target_Now(int sock,int type){
    char ERreq[4] = {0x05,type,0x00,0x01};
    int reqlen = 4;
    int write_size;
    write_size = API_socket_send(sock,ERreq,reqlen);
    return 1;
}

int Say_Cannot_Build_Target_Reason(int sock){
#ifdef WIN32
    int WinErrno = WSAGetLastError();
    switch (WinErrno){
        case WSAENETUNREACH:
            Say_Cannot_Build_Target_Now(sock,NET_NOT_REACHABLE);
            break;
        case WSAEHOSTUNREACH:
            Say_Cannot_Build_Target_Now(sock,THE_HOST_NOT_REACHABLE);
            break;
        case WSAECONNREFUSED:
            Say_Cannot_Build_Target_Now(sock,REFUSE_BY_REMOTE);
            break;
        case WSAETIMEDOUT:
            Say_Cannot_Build_Target_Now(sock,TTL_OVERTIME);
            break;
    }
#else
    switch(errno){
        case ENETUNREACH: 
            Say_Cannot_Build_Target_Now(sock,NET_NOT_REACHABLE);
            break;
        case EHOSTUNREACH:
            Say_Cannot_Build_Target_Now(sock,THE_HOST_NOT_REACHABLE);
            break;
        case ECONNREFUSED:
            Say_Cannot_Build_Target_Now(sock,REFUSE_BY_REMOTE);
            break;
        case ETIMEDOUT:
            Say_Cannot_Build_Target_Now(sock,TTL_OVERTIME);
            break;
    }
#endif
    return 1;
}

int Say_Build_Target_OK(int sock){
    char ERreq[10] = {0x05,0x00,0x00,0x01,0x41,0x41,0x41,0x41,0x41,0x41};
    int reqlen = 10;
    int write_size;
    write_size = API_socket_send(sock,ERreq,reqlen);
    return 1;
}

//#define GET_TARGET_SOCKET_ERROR   -1

int Get_Target_Socket(int sock){
    int des_port;
    unsigned int addrvalue;
    char addrURL[300];
    struct in_addr buf_addr;
    char buffer[2000],buf[2000];
    int read_size,write_size,url_len;
    int mode; int cli_sock;
    // 2. Request
    read_size = API_socket_recv( sock , buffer,4);
    mode = buffer[1];
    // 2.1 read the ip or url
    if(buffer[3] == IPV4_TYPE){
        read_size = API_socket_recv(sock , (char *)&addrvalue , 4 );
        buf_addr.s_addr = addrvalue;
        strcpy(addrURL, inet_ntoa(buf_addr));
        printf("the recv ip is %s",addrURL);
    }
    else if(buffer[3] == URL_TYPE){
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
    else if(buffer[3] == IPV6_TYPE) {
        Say_Cannot_Build_Target_Now(sock,UNASSIGNED_TYPE);
printf("Not support IPv6?\n");
        return GET_TARGET_SOCKET_ERROR;
    }
    else{
        Say_Cannot_Build_Target_Now(sock,UNASSIGNED_TYPE);
printf("NOT IPv4 IPv6 and URL ?\n");
        return GET_TARGET_SOCKET_ERROR;
    }
    // 2.2 read the target port
    read_size = API_socket_recv(sock,buffer,2);
    if(read_size != 2){
        Say_Cannot_Build_Target_Now(sock,UNASSIGNED_TYPE);
        return GET_TARGET_SOCKET_ERROR;
    }
    des_port = (unsigned char) buffer[0]*256 +
         (unsigned char)buffer[1];

    // 2.3 connect URL : port
    if(mode == TCP_TYPE){
        printf (" Tcp ---> %s:%d \n",addrURL,des_port);
        cli_sock = API_socket_connect(addrURL,des_port);
    }
    else{
        Say_Cannot_Build_Target_Now(sock,NOT_SUPPORT_CMD);
printf("Not support  UDP?\n");
        return GET_TARGET_SOCKET_ERROR;
    }
    if(cli_sock == SOCKET_CONNECT_ERROR){
        Say_Cannot_Build_Target_Reason(sock);
        return GET_TARGET_SOCKET_ERROR;
    }
    Say_Build_Target_OK(sock);
    return cli_sock;
}



//#define BUILD_TARGET_SOCKET_ERROR -1

int Build_Target_Socket(int sock){
    int cli_sock;
    if(Check_Version(sock) == CHECK_VERSION_OK){
        cli_sock = Get_Target_Socket (sock);
        if(cli_sock != GET_TARGET_SOCKET_ERROR){
            return cli_sock;
        }
    }
    return BUILD_TARGET_SOCKET_ERROR;
}
