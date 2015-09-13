#ifndef _NODE_CONN_PROTOCOL_H_
#define _NODE_CONN_PROTOCOL_H_
//------------------------------------------------------
#include "PROTOCOL_FORMAT.h"

#define NODECONN_PROTOCOL_ANALYSIS_FOR_SERVER_ERROR  1
#define NODECONN_PROTOCOL_ANALYSIS_FOR_SERVER_OK     2
int NodeConn_Analysis_For_Server(int client_sock,char *CMD_MESSAGE,int CMD_LEN);


#define NODECONN_NODE_CONNECT_OK          1
#define NODECONN_NODE_CONNECT_ERROR       2
int NodeConn_Node_Connect(char *URL,int port);

#endif
