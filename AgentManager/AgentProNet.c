/**
* File Name :AgentManager/AgentProNet.c
* Author: rootkiter
* E-mail: rootkiter@rootkiter.com
* Created Time : 一 12/28 20:14:42 2015
*/
#include "AgentProNet.h"

//===========================================================
//   inline  function 
//      m_SendData_CmdTunnel( conn,data, data, len);
#define M_SENDDATA_CMDTUNNEL_ERROR   -1
#define M_SENDDATA_CMDTUNNEL_OK       1
int m_SendData_CmdTunnel(pPCConn *conn,char *data,int datalen){
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
    nsend = API_socket_send(cmdsocket,data,datalen);
    // unlock it 
    conn->BusyType = NET_SESSION_UNUSED_NOW;
    // check send state
    if(nsend != datalen){
        return M_SENDDATA_CMDTUNNEL_ERROR;
    }
    return M_SENDDATA_CMDTUNNEL_OK;
}

//===========================================================

#define MAX_PROTO_BUFLEN   200
int PROTO_SendProto(pPCConn conn,pAgent_proto proto){
    char cmdbuff[MAX_PROTO_BUFLEN];
    if(conn == NULL || proto == NULL ) {
        return PROTO_SENDPROTO_ERROR;
    }
    if(proto->argLen + 24 > MAX_PROTO_BUFLEN){
        Printf_Error("PROTOBUFLEN_OVERFLOW !!!!!");
        return PROTO_SENDPROTO_ERROR;
    }
    API_m_itochar(proto->cmdType,&(cmdbuff[0])     ,4);
    API_m_itochar(proto->cmdID  ,&(cmdbuff[4])     ,4);
    API_m_itochar(proto->jobID  ,&(cmdbuff[8])     ,4);
    API_m_itochar(proto->fromID ,&(cmdbuff[12])    ,4);
    API_m_itochar(proto->toID   ,&(cmdbuff[16])    ,4);
    API_m_itochar(proto->argLen ,&(cmdbuff[20])    ,4);
    memcpy(&(cmdbuff[24]),proto->cmdargs,MAX_PROTO_BUFLEN - 24);
    if(m_SendData_CmdTunnel(conn) == M_SENDDATA_CMDTUNNEL_ERROR){
        Printf_Error("PROTO_SENDPROTO_ERROR ");
        return PROTO_SENDPROTO_ERROR;
    }
    return PROTO_SENDPROTO_OK;
}

int PROTO_RecvProto(pPCConn conn,pAgent_proto proto){
    int nrecv;
    char cmdbuff[MAX_PROTO_BUFLEN];
    if(conn == NULL || proto == NULL){
        return PROTO_GETPROTO_ERROR;
    }
    nrecv = API_socket_recv(conn->cmd_socket,cmdbuff,MAX_PROTO_BUFLEN);
    if(nrecv == 0){ return PROTO_GETPROTO_NOTHING; }
    if(nrecv != MAX_PROTO_BUFLEN){
        return PROTO_GETPROTO_ERROR;
    }
    proto->cmdType = API_m_chartoi(&(cmdbuff[ 0]),4);
    proto->cmdID   = API_m_chartoi(&(cmdbuff[ 4]),4);
    proto->jobID   = API_m_chartoi(&(cmdbuff[ 8]),4);
    proto->fromID  = API_m_chartoi(&(cmdbuff[12]),4);
    proto->toID    = API_m_chartoi(&(cmdbuff[16]),4);
    proto->argLen  = API_m_chartoi(&(cmdbuff[20]),4);
    memncpy(proto->cmdargs,&(cmdbuff[24]),MAX_PROTO_BUFLEN-24);
    return PROTO_GETPROTO_OK;
}

int PROTO_RecvState(pPCConn conn){
    int res = API_socket_read_state(conn->cmd_socket);
    if(res == SOCKET_CAN_READ_STATE ){
        return PROTO_RECVSTATE_CANRECV;
    }
    else if ( SOCKET_OVER_TIME == res ){
        return PROTO_RECVSTATE_OVERTIME;
    }
    return PROTO_RECVSTATE_SOCKET_ERROR;
}

///=======================================================
///   PCConn Functions
///=======================================================
pPCConn PCCONN_CreatePCConnFromSocket(int sock){
    pPCConn conn = (pPCConn)malloc(sizeof(PCConn));
    if(conn == NULL){
        return PCCONN_CREATEPCCONNFROMSOCKET_ERROR;
    }
    conn->ConnType   = CONNTYPE_DIRECT_CONNECT;
    conn->cmd_socket = sock;
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
    if(conn = NULL){
        return PCCONN_COPY_ERROR;
    }
    conn1 = (pPCConn)malloc(sizeof(PCConn));
    if(conn1 = NULL){
        return PCCONN_COPY_ERROR;
    }
    conn1->ConnType   = conn->ConnType;
    conn1->BusyType   = conn->BusyType;
    conn1->port       = conn->port;
    conn1->cmd_socket = conn->cmd_socket;
    memcpy(conn1->IPaddr,conn->IPaddr,MAX_IP_ADDR_LEN);
}
