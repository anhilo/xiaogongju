#include"Protocol.h"

int Protocol_Analysis_For_Server(
    int client_sock,
    MY_SOCKADDR addr,
    MY_ADDRLEN  len
){
    int new_node_conn = -1;
    char *CMD_MESSAGE = (char*)malloc(sizeof(char)*PROTOCOL_MAX_LEN);
    int analy_result = -2;
    API_socket_recv(client_sock,CMD_MESSAGE,PROTOCOL_MAX_LEN);
    new_node_conn = CMD_MESSAGE[PROTOCOL_TYPE_OFFSET];  // 获取顶层协议值
    switch(new_node_conn){
        case NEW_NODE_CONN:
            // 新节点链接协议，交给新节点链接协议进行处理
            analy_result = NodeConn_Analysis_For_Server(client_sock,CMD_MESSAGE,
                PROTOCOL_MAX_LEN);
            if(analy_result == NODECONN_PROTOCOL_ANALYSIS_FOR_SERVER_ERROR){
                Printf_Error(" NODECONN_PROTOCOL_ANALYSIS_FOR_SERVER_ERROR ");
            }
            break;
    }
    return 1;
}

int PRO_start_server(int port,int max_num){
    int ser_sock = API_socket_init_server(port,max_num);
    API_socket_server_start(    
        ser_sock,
        Protocol_Analysis_For_Server
    );
    return 1;
}

int PRO_socket_send(int sock,char *buffer,int buflen){
    return API_socket_send(sock,buffer,buflen);
}

int PRO_socket_recv(int sock,char *buffer,int buflen){
    return API_socket_recv(sock,buffer,buflen);
}
