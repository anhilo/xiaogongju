/**
* File Name :AgentManager/PCConn.c
* Author: rootkiter
* E-mail: rootkiter@rootkiter.com
* Created Time : Tue 05 Jan 2016 01:06:57 AM PST
*/
#include "PCConn.h"


//===========================================================
//   inline  function 
//      m_SendData_CmdTunnel( conn,data, data, len);
#define M_SENDDATA_CMDTUNNEL_ERROR   -1
#define M_SENDDATA_CMDTUNNEL_OK       1
int m_SendData_CmdTunnel(pPCConn conn,char *data,int datalen){
    int nsend = -1;
    int cmdsocket = -1;
    if(conn == NULL || conn->cmd_socket == -1){
        return M_SENDDATA_CMDTUNNEL_ERROR;
    }
    // get cmd_socket
    cmdsocket = conn->cmd_socket;
    // session busylock
    while(NET_SESSION_BUSY_NOW == conn->BusyType){
        MIC_USLEEP(10);
    }
    // session lock it
    conn->BusyType = NET_SESSION_BUSY_NOW;
    // send data
//Printf_DEBUG("send data len is %d ",datalen);
    nsend = API_socket_send(cmdsocket,data,datalen);
MIC_USLEEP(50);
    // unlock it 
    conn->BusyType = NET_SESSION_UNUSED_NOW;
    // check send state
    if(nsend != datalen){
        return M_SENDDATA_CMDTUNNEL_ERROR;
    }
    return M_SENDDATA_CMDTUNNEL_OK;
}

///=======================================================
///   PCConn Functions
///=======================================================
pPCConn PCCONN_CreatePCConnFromSocket(int sock){
    pPCConn conn = (pPCConn)malloc(sizeof(PCConn));
    if(conn == NULL){
        return PCCONN_CREATEPCCONNFROMSOCKET_ERROR;
    }
    conn->ConnType   = CONNTYPE_REVERSE_CONNECT;
    conn->cmd_socket = sock;
    conn->BusyType   = NET_SESSION_UNUSED_NOW;
    return conn;
}

pPCConn PCCONN_CreatePCConnFromIPPort(char *ip,int port,int sock){
    pPCConn conn = PCCONN_CreatePCConnFromSocket(sock);
    if( PCCONN_CREATEPCCONNFROMSOCKET_ERROR == 
        conn){
        return PCCONN_CREATEPCCONNFROMIPPORT_ERROR;
    }
    conn->ConnType   = CONNTYPE_DIRECT_CONNECT;
    strncpy(conn->IPaddr,ip,MAX_IP_ADDR_LEN);
    conn->port       = port;
    conn->BusyType   = NET_SESSION_UNUSED_NOW;
    return conn;
}

int PCCONN_Free(pPCConn conn){
    if(conn == NULL){
        return PCCONN_FREE_ERROR;
    }
    free(conn);
    conn = NULL;
    return PCCONN_FREE_OK;
}

pPCConn PCCONN_Copy(pPCConn conn){
    pPCConn conn1 = NULL;
    if(conn == NULL){
        return PCCONN_COPY_ERROR;
    }
    conn1 = (pPCConn)malloc(sizeof(PCConn));
    if(conn1 == NULL){
        return PCCONN_COPY_ERROR;
    }
    conn1->ConnType   = conn->ConnType;
    conn1->BusyType   = conn->BusyType;
    conn1->port       = conn->port;
    conn1->cmd_socket = conn->cmd_socket;
    memcpy(conn1->IPaddr,conn->IPaddr,MAX_IP_ADDR_LEN);
    return conn1;
}

int PCCONN_SendData(pPCConn conn,char *data,int datalen){
    int res = m_SendData_CmdTunnel(conn,data,datalen);
    if(res == M_SENDDATA_CMDTUNNEL_ERROR){
        return PCCONN_SENDDATA_ERROR;
    }
    return datalen;
}

int PCCONN_RecvData(pPCConn conn,char *data,int maxlen){
    int nrecv;
    char cmdbuff[MAX_PROTO_BUFLEN];
    if( conn == NULL ){
        return PCCONN_RECVDATA_ERROR;
    }
MIC_USLEEP(10);
    nrecv = API_socket_recv(conn->cmd_socket,cmdbuff,maxlen);
    if(nrecv == 0){ return PCCONN_RECVDATA_ERROR; }
    if(nrecv > maxlen){
        Printf_Error("nrecv (%d) > maxlen (%d)",
            nrecv,maxlen);
        return PCCONN_RECVDATA_ERROR;
    }
    memcpy(data,cmdbuff,maxlen);
    return nrecv;
}

pPCConn PCCONN_Connect(char *ip,int port){
    int socket = SOCKET_CONNECT_ERROR;
    pPCConn conn = NULL;
    if(ip == NULL || port == -1){
        return PCCONN_CONNECT_ERROR;
    }
    socket = API_socket_connect(ip,port);
    if(socket == SOCKET_CONNECT_ERROR){
        return PCCONN_CONNECT_ERROR;
    }
    conn = PCCONN_CreatePCConnFromIPPort
        (ip,port,socket);
    if(conn == PCCONN_CREATEPCCONNFROMSOCKET_ERROR){
        return PCCONN_CONNECT_ERROR;
    }
    return conn;
}

int PCCONN_Conn_2_Conn(pPCConn conn1,pPCConn conn2,int usec){
    int sock1,sock2;
    if(conn1 == NULL || conn2 == NULL){
        return PCCONN_CONN_2_CONN_ERROR;
    }
    sock1 = conn1->cmd_socket;
    sock2 = conn2->cmd_socket;
    int res = tunn_sock_to_sock(sock1,sock2,usec);
    if(TUNNEL_SOCK_TO_SOCK_ERROR == res){
        return PCCONN_CONN_2_CONN_ERROR;
    }
    return PCCONN_CONN_2_CONN_OK;
}
