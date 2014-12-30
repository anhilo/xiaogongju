#include "BaseAPI.h"

int API_env_init(){
#ifdef WIN32
    // 初始化套接字库 win32 下需要
    WSADATA wsd;
    if (WSAStartup(MAKEWORD(2,2), &wsd) != 0)
    {
        perror ("WSAStartup failed!");
        return 1;
    }
#else
    signal(SIGPIPE, SIG_IGN);
#endif
    return ENV_INIT_OK;
}

int API_socket_connect(char *ser_addr,int port){
    struct sockaddr_in server_addr,client_addr;
    struct hostent * des_addr;
    memset(&client_addr, 0 ,sizeof(client_addr));
    //bzero(&client_addr,sizeof(client_addr));
    client_addr.sin_family = AF_INET; // internet
    client_addr.sin_addr.s_addr = htons( INADDR_ANY ); // local address
    client_addr.sin_port = htons(0); // any port
    int client_socket = socket(AF_INET,SOCK_STREAM,0);
    if( client_socket < 0){
        printf("Create Socket Failed ! \n");
        return SOCKET_CONNECT_ERROR;
    }
    if( bind(client_socket,(struct sockaddr*)&client_addr,sizeof(client_addr))){
        printf("Client Bind Port Failed ! \n");
        return SOCKET_CONNECT_ERROR;
    }
    memset(&server_addr,0,sizeof(server_addr));
    //bzero(&server_addr,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    // dns url -> ip
    des_addr = API_socket_gethostbyname(ser_addr);
    server_addr.sin_addr = *(struct in_addr*)des_addr->h_addr;
    if( server_addr.sin_addr.s_addr == 0 )
    {
        printf("Server IP Address Error!\n");
        return SOCKET_CONNECT_ERROR;
    }
    server_addr.sin_port = htons(port);
    //向服务器发起连接,连接成功后client_socket代表了客户机和服务器的一个socket连接
    if(connect(client_socket,(struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("Can Not Connect To %s!\n", ser_addr );
        return SOCKET_CONNECT_ERROR;
    }
    //puts("got the client_socket");
    return client_socket;
}

int API_socket_init_server(int port,int maxlisten){
    int socket_desc;
    struct sockaddr_in server;
    int c;
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
        return SOCKET_SERVER_INIT_ERROR;
    }
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( port );
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return SOCKET_SERVER_INIT_ERROR;
    }
    // puts("bind done");
    //Listen
    listen(socket_desc , maxlisten);
    return socket_desc;
}

struct hostent *API_socket_gethostbyname(char * ser_addr){
    return gethostbyname(ser_addr); 
}

int API_socket_read_state(int sock,int sec,int usec){
    fd_set fds ;
    int n = 1;
    struct timeval tv ;
    // Set up the file descriptor set.
    FD_ZERO(&fds) ;
    FD_SET(sock, &fds) ;
    // Set up the struct timeval for the timeout.
    tv.tv_sec = sec ;
    tv.tv_usec = usec ;
    // Wait until timeout or data received.
    n = select ( sock + 1, &fds, NULL, NULL, &tv ) ;
    if ( n >0 ){
        if ( FD_ISSET(sock,&fds) ) {
            return SOCKET_CAN_READ_STATE; // socket 可以读
        }
        return SOCKET_MAY_CLOSE_NOW;
    }
    else if ( n == 0 ) return SOCKET_OVER_TIME; // 超时
    return SOCKET_ERROR; // 出错了
}

int API_socket_write_state(int sock,int sec,int usec){
    fd_set fds ;
    int n = 1;
    struct timeval tv ;
    // Set up the file descriptor set.
    FD_ZERO(&fds) ;
    FD_SET(sock, &fds) ;
    // Set up the struct timeval for the timeout.
    tv.tv_sec = sec ;
    tv.tv_usec = usec ;
    // Wait until timeout or data received.
    n = select ( sock + 1, NULL, &fds, NULL, &tv ) ;

    if ( n >0 ){
        if ( FD_ISSET(sock,&fds) ) {
            return SOCKET_CAN_WRITE_STATE; // socket 可以读
        }
        return SOCKET_MAY_CLOSE_NOW;
    }
    else if ( n == 0 ) return SOCKET_OVER_TIME; // 超时
    return SOCKET_ERROR; // 出错了
}


int API_socket_send(int sock,char *buf,int buflen){
    return send(sock,buf,buflen,0);
}

int API_socket_recv(int sock,char *buf,int buflen){
    return recv(sock,buf,buflen,0);
}

int API_socket_close(int sock){
    f (sock>0){
#ifdef WIN32
        closesocket(sock);
#else
        close(sock);
#endif
    }
    return 1;
}



int API_thread_create(pthread_t *thread, const pthread_attr_t *attr,
                          void *(*start_routine) (void *), void *arg){
    return pthread_create(thread,attr,start_routine,arg);
}

int API_thread_detach(){
    return pthread_detach(pthread_self());
}


int API_m_itochar(int a,char *b,int len){
    int i;
    if(len!=4) return I_TO_CHAR_ERROR;
    int buf = a;
    for (i = 0;i< len ;i++){
        b[i] = buf % 128;
        buf = buf / 128;
    }
    return I_TO_CHAR_OK;
}

int API_m_chartoi(char *b,int len){
    int i;
    int result = 0;
    if (len!=4)return CHAR_TO_I_ERROR;
    for (i = len - 1 ;i >= 0 ; i--){
        result = result * 128 + b[i];
    }
    return result;
}

