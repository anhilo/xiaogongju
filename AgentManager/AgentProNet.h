#ifndef _AGENTPRONET_H_
#define _AGENTPRONET_H_

#define MAX_IP_ADDR_LEN    20

#define CONNTYPE_DIRECT_CONNECT   1
#define CONNTYPE_REVERSE_CONNECT  2
#define NET_SESSION_BUSY_NOW      1
#define NET_SESSION_UNUSED_NOW    2
typedef struct PCConnInfo{
    int ConnType;
    int BusyType;   // BusyType
    char IPaddr[MAX_IP_ADDR_LEN];
    int port;
    int cmd_socket;
}PCConn,*pPCConn;

typedef struct CBFNode{
    int cmdtype;
    int cmdid;
    pAgentProto_CBF *cbf;
}CBFNode,*pCBFNode;

typedef struct CBFLIST{
    int    CBFListMaxLen;
    int    CBFListLen;
    pCBFNode *cbf;
}CBF_list,*pCBF_list;

#define PROTO_SENDPROTO_ERROR         -1
#define PROTO_SENDPROTO_OK             1
int PROTO_SendProto(pPCConn conn,pAgent_proto proto);

//  recv about  function
#define PROTO_ADDPROTOCOLCBF_ERROR    -1
#define PROTO_ADDPROTOCOLCBF_OK        1
int PROTO_AddProtocolCBF(CBF_list *list,int cmdtype,int cmdid,pAgentProto_CBF fun);

#define PROTO_GETPROTO_ERROR    0 //出现错误
#define PROTO_GETPROTO_OK       1 //收到了消息
#define PROTO_GETPROTO_NOTHING  2 //未收到消息
int PROTO_RecvProto(pPCConn conn,pAgent_proto proto);

#define PROTO_PARSE_ANALYSIS_DO_ERROR -1
#define PROTO_PARSE_ANALYSIS_DO_OK     1
int PROTO_Parse_Analysis_Do(CBF_list *list,pAgent_proto proto);

#endif
