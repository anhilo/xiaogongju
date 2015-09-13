#include "Node_Conn_Protocol.h"

/**
    +---------------+--------------+---------------+
    | NEW_NODE_CONN | CONN_MESSAGE | PROTOCOL_DATA |
    +---------------+--------------+---------------+
*/
#define CONN_MESSAGE_OFFSET       1   // 连接信息偏移
#define CONN_MESSAGE_DATA_OFFSET  2   // 连接时携带的参数
    #define CONN_MESSAGE_N_CONNECT           1 // node   -> server  节点向服务发起连接请求
    #define CONN_MESSAGE_S_I_HAVE_ADMIN      2 // server -> node    管理员和我在一起，从现在开始你听我的
    #define CONN_MESSAGE_S_NO_ADMIN          3 // server -> node    管理员没在这里（连接被接受）
    #define CONN_MESSAGE_N_I_HAVE_ADMIN      4 // node   -> server  管理员在我这里，从现在起你听我的
    #define CONN_MESSAGE_N_NO_ADMIN          5 // node   -> server  我也没和管理员在一起，我们现在是孤立节点
    #define CONN_MESSAGE_A_ALONE_CHECK       6 // server -> node    孤立节点确认，共同等待，管理员的连接。

    #define CONN_MESSAGE_N_IM_ADMIN          7 // node   -> server  我是管理员，你们要听我的
    #define CONN_MESSAGE_S_CHALLENGE         8 // server -> node    给管理员一段随机数据，要求其进行数字签名
    #define CONN_MESSAGE_N_ACK_CHALLENGE     9 // node   -> server  这是签名结果，你验签吧
    #define CONN_MESSAGE_S_CHALLENGE_OK     10 // server -> node    验签成功，你确实是管理员
    #define CONN_MESSAGE_S_CHALLENGE_ERROR  11 // server -> node    
    //    验签失败，你是伪造的。我拒绝听你的。（获取该节点的ip信息，本地留存，并在合适的时间通知管理员）
    #define CONN_MESSAGE_N_I_GOT_IT         12 // node   -> server  我将开始管理工作，请配合。 
    #define CONN_MESSAGE_S_I_GOT_IT         13 // node   -> server  我将开始正常工作，请配合。 


//*********************************************************************************************************
//=============================================================
#define NODECONN_SEND_OK    1
#define NODECONN_SEND_ERROR 2
//----------------------------------------------------
int NodeConn_Send(int sock,int MESSAGE_CMD,char *buf,int buflen){
    char CMD_BUF[PROTOCOL_MAX_LEN];
    int  send_size;
    CMD_BUF[PROTOCOL_TYPE_OFFSET] = NEW_NODE_CONN;
    CMD_BUF[ PROTOCOL_TYPE_OFFSET + CONN_MESSAGE_OFFSET ] = MESSAGE_CMD;
    memcpy(
        (&(CMD_BUF[ PROTOCOL_TYPE_OFFSET + CONN_MESSAGE_DATA_OFFSET])),
        buf,
        buflen
    );
    send_size = PRO_socket_send(sock,CMD_BUF,PROTOCOL_MAX_LEN);
    if(send_size != PROTOCOL_MAX_LEN) { return NODECONN_SEND_ERROR; }

    return NODECONN_SEND_OK;
}
//----------------------------------------------------

//=============================================================
//----------------------------------------------------
int NodeConn_Recv(int sock,char *buf,int maxlen){
    return PRO_socket_recv(sock,buf,maxlen);
}
//----------------------------------------------------

//*********************************************************************************************************

//=============================================================
#define NODECONN_ACK_SEND_OK      1
#define NODECONN_ACK_SEND_ERROR   2
//----------------------------------------------------
int NodeConn_ACK_Send(int sock,int MESSAGE_CMD){
    char buf[15];
    int sendSTATE = NodeConn_Send(sock,MESSAGE_CMD,buf,0);
    if(sendSTATE == NODECONN_SEND_OK){
        return NODECONN_ACK_SEND_OK;
    }
    return NODECONN_ACK_SEND_ERROR;
}
//----------------------------------------------------

//=============================================================
#define NODECONN_CHECK_RECV_OK      1
#define NODECONN_CHECK_RECV_ERROR   2
//----------------------------------------------------
int NodeConn_check_recv(char* CMDBUF ,int MESSAGE_CMD){
    int conn_pro,cmd_msg;
    conn_pro = CMDBUF[PROTOCOL_TYPE_OFFSET  ];
    cmd_msg  = CMDBUF[PROTOCOL_TYPE_OFFSET + CONN_MESSAGE_OFFSET  ];
    
    if(conn_pro != NEW_NODE_CONN || cmd_msg != MESSAGE_CMD){
        return NODECONN_CHECK_RECV_ERROR;
    }
    return NODECONN_CHECK_RECV_OK;
}

//=============================================================
#define NODECONN_NORNAL_ACK_OK    1
#define NODECONN_NORNAL_ACK_ERROR 2
//----------------------------------------------------
int NodeConn_Nomal_ACK(int sock){
    // 普通的 被控节点，正常回复即可
    char CMD_BUF[PROTOCOL_MAX_LEN];
    int haveAdmin = GLOBAL_Admin_State();
    int sendSTATE;
    int nrecv,nsend;
    if(haveAdmin == GLOBAL_ADMIN_ONLINE){
            // 管理员在服务器这一方，所以正常告知新节点即可
Printf_OK("ADMIN on server node");
        if( NODECONN_ACK_SEND_OK !=
         NodeConn_ACK_Send(sock,CONN_MESSAGE_S_I_HAVE_ADMIN)){
            return NODECONN_NORNAL_ACK_ERROR;
        }
        nrecv = NodeConn_Recv(sock,CMD_BUF,PROTOCOL_MAX_LEN);
        if(nrecv != PROTOCOL_MAX_LEN ||
            NODECONN_CHECK_RECV_ERROR == 
            NodeConn_check_recv(CMD_BUF,CONN_MESSAGE_S_I_GOT_IT)
        ){
            return NODECONN_NORNAL_ACK_OK;
        }
    }
    else if(haveAdmin == GLOBAL_ADMIN_OFFLINE){
            // 管理员不再服务器这一方，则需要询问是否在客户端那一方
        if( NODECONN_ACK_SEND_OK !=
         NodeConn_ACK_Send(sock,CONN_MESSAGE_S_NO_ADMIN)
        ){
            return NODECONN_NORNAL_ACK_ERROR;
        }

        nrecv = NodeConn_Recv(sock,CMD_BUF,PROTOCOL_MAX_LEN);

        if(nrecv == PROTOCOL_MAX_LEN ){
            if(
                NODECONN_CHECK_RECV_OK == 
                NodeConn_check_recv(CMD_BUF,CONN_MESSAGE_N_I_HAVE_ADMIN)
            ){  // 管理员在客户端，需要开始认证合法性，并同步自身数据
    //HKOVERTIME------------------------------------------------------------------
    //----------------------------------------------------------------------------
                Printf_OK("The Client have admin , I'll send my info there");
                nsend = NodeConn_Send(sock,CONN_MESSAGE_S_I_GOT_IT,CMD_BUF,0);
            }
            else if( NODECONN_CHECK_RECV_OK == 
                NodeConn_check_recv(CMD_BUF,CONN_MESSAGE_N_NO_ADMIN)
            ){
                // 客户端也没有管理员，我们现在是孤立节点
                nsend = NodeConn_Send(sock,CONN_MESSAGE_A_ALONE_CHECK,CMD_BUF,0);
                Printf_OK("No Admin  , all a lone");
            }

            return NODECONN_NORNAL_ACK_OK;
        }
    }

    return NODECONN_NORNAL_ACK_OK;
}
//----------------------------------------------------

int NodeConn_Analysis_For_Server(int client_sock,char *CMD_MESSAGE,int CMD_LEN){
    int new_node_conn ,conn_message;
    int ret_state;
    new_node_conn = CMD_MESSAGE[PROTOCOL_TYPE_OFFSET];  // 获取顶层协议值
    if(CMD_LEN != PROTOCOL_MAX_LEN || new_node_conn != NEW_NODE_CONN){
        Printf_Error("PROTOCOL_TYPE = %d \n",new_node_conn);
        return CALLBACK_FUN_RUN_ERROR;
    }
    conn_message = CMD_MESSAGE[ PROTOCOL_TYPE_OFFSET + CONN_MESSAGE_OFFSET] ;
    switch(conn_message){
        case CONN_MESSAGE_N_CONNECT:
            // 节点向服务发起连接请求
            ret_state = NodeConn_Nomal_ACK(client_sock);
            if(ret_state == NODECONN_NORNAL_ACK_OK){
                return NODECONN_PROTOCOL_ANALYSIS_FOR_SERVER_OK;
            }
            return NODECONN_PROTOCOL_ANALYSIS_FOR_SERVER_ERROR;
            break;
        case CONN_MESSAGE_N_IM_ADMIN:
            // 我是管理员，你们要听我的
            break;
    }
    return NODECONN_PROTOCOL_ANALYSIS_FOR_SERVER_ERROR;
}

int NodeConn_Node_Connect(char *URL,int port){
    int sock; char buf[15];
    int nsend,nrecv;
    sock = API_socket_connect(URL,port);
    if(sock == SOCKET_INIT_ERROR){
        Printf_Error("Connect %s:%d",URL,port);
        return NODECONN_NODE_CONNECT_ERROR;
    }

    nsend =NodeConn_Send(sock,CONN_MESSAGE_N_CONNECT , buf,0);
    if(nsend != NODECONN_SEND_OK){
        return NODECONN_NODE_CONNECT_ERROR;
    }

    nrecv = NodeConn_Recv(sock,buf,PROTOCOL_MAX_LEN);
    if(nrecv != PROTOCOL_MAX_LEN){
        Printf_Error("nrecv error Node Connect");
        return NODECONN_NODE_CONNECT_ERROR;
    }

    if( NODECONN_CHECK_RECV_OK == 
        NodeConn_check_recv(buf,CONN_MESSAGE_S_I_HAVE_ADMIN)
    ){   // 管理员在服务器那头，我要去验证管理员的合法性并同步数据
    //HKOVERTIME------------------------------------------------------------------
Printf_DEBUG("Admin on Server node");
    //----------------------------------------------------------------------------
        nsend = NodeConn_ACK_Send(sock,CONN_MESSAGE_N_I_GOT_IT);
        if(nsend != NODECONN_SEND_OK){
            Printf_Error("Admin there ,But ACK back error");
            return NODECONN_NODE_CONNECT_ERROR;
        }
        return NODECONN_NODE_CONNECT_OK;
    }
    
    if( NODECONN_NODE_CONNECT_OK == 
        NodeConn_check_recv(buf,CONN_MESSAGE_S_NO_ADMIN)
    ){  // 管理员不在那一头，我要检查本地是否有管理员
        if(GLOBAL_ADMIN_ONLINE == GLOBAL_Admin_State()){
                // 本地有管理员，我要告诉服务器我有管理员
Printf_DEBUG("Admin on Client node");
            nsend = NodeConn_Send(sock,CONN_MESSAGE_N_I_HAVE_ADMIN,buf,0);
            nrecv = NodeConn_Recv(sock,buf,PROTOCOL_MAX_LEN);
            if(NODECONN_CHECK_RECV_OK == 
                NodeConn_check_recv(buf,CONN_MESSAGE_S_I_GOT_IT)
            ){
                Printf_OK("Admin here, told the server ok");
                return  NODECONN_NODE_CONNECT_OK;
            }
        }
        else{
            // 本地也没有管理员，我们现在是孤立节点了
Printf_DEBUG("No Admin ");
            nsend = NodeConn_Send(sock,CONN_MESSAGE_N_NO_ADMIN,buf,0);
            nrecv = NodeConn_Recv(sock,buf,PROTOCOL_MAX_LEN);
            if(NODECONN_CHECK_RECV_OK == 
                NodeConn_check_recv(buf,CONN_MESSAGE_A_ALONE_CHECK)
            ){
                Printf_OK("No Admin  , all a lone");
                return  NODECONN_NODE_CONNECT_OK;
            }
            
        }
    }
    return NODECONN_NODE_CONNECT_OK;
}
