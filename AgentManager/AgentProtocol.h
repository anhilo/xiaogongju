#ifndef _AGENTPROTOCOL_H_
#define _AGENTPROTOCOL_H_

#include"../generic.h"

#define MAX_ARG_LEN   100
typedef struct AGENTPROTOCOL{
    int cmdType;
    int cmdID;
    int jobID;      // cmd section
    int fromID;
    int toID;       // address section
    int argLen;
    char *cmdargs;  // arg section
}Agent_proto,*pAgent_proto;

typedef int (*pAgentProto_CBF) (int argc,char *argv);

#define PROTO_INIT_ERROR              -1
#define PROTO_INIT_OK                  1
int PROTO_Init();   // init cbfnode list

//  send about function
#define PROTO_CREATEPROTO_ERROR        -1
#define PROTO_CREATEPROTO_OK            1
int PROTO_CreateProto(pAgent_proto proto,int cmdtype,int cmdid,int jobid);

#define PROTO_SETADDRESS_ERROR        -1
#define PROTO_SETADDRESS_OK            1
int PROTO_SetAddress (pAgent_proto proto,int fromid,int toid);

#define PROTO_SETARGS_ERROR           -1
#define PROTO_SETARGS_OK               1
int PROTO_SetArgs (pAgent_proto proto,int arglen,char *cmdargs);

#define PROTO_FREEPROTO_ERROR     -1
#define PROTO_FREEPROTO_OK         1
int PROTO_FreeProto(pAgent_proto proto);

#endif
