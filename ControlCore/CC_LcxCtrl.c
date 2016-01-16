/**
* File Name :ControlCore/CC_LcxCtrl.c
* Author: rootkiter
* E-mail: rootkiter@rootkiter.com
* Created Time : 六  1/16 13:14:09 2016
*/
#include "CC_LcxCtrl.h"
#include "../AgentManager/PCConn.h"
#include "Cmd_Ctrl.h"

typedef struct lcx_tran_struct{
    int targetid;
    char rip[MAX_IP_ADDR_LEN];
    int rport;
    int usec;
}LcxTranStruct,*pLcxTranStruct;



int fun_callback_for_StartLcxTran(pPCConn conn,char *value){
    pLcxTranStruct pvalue = (pLcxTranStruct)value;
    if(pvalue == NULL){
        return 0;
    }
    int targetid = pvalue -> targetid;
    int rport    = pvalue -> rport;
    char rip[MAX_IP_ADDR_LEN];
    int usectime = pvalue -> usec;
    strncpy(rip,pvalue->rip,MAX_IP_ADDR_LEN);
    
    pPCConn conn2 = CMDCTRL_BuildTargetSock(targetid,
        AGENT_SERVER_COMMAND_LCXTRAN);
    if(conn2 == CMDCTRL_BUILDTARGETSOCK_ERROR){
        Printf_DEBUG("Build Lcx_tran Tunnel Error --> %d -> [%s:%d]",
            targetid,rip,rport);
        return 0;
    }
    int bufferlen = (sizeof(char)*MAX_IP_ADDR_LEN + sizeof(int));
    char *sendbuffer = (char *)malloc(bufferlen);
    memcpy(sendbuffer,rip,MAX_IP_ADDR_LEN);
    API_m_itochar(rport,&(sendbuffer[MAX_IP_ADDR_LEN]),4);
    int res = PCCONN_SendData(conn2,sendbuffer,bufferlen);
    if(res == PCCONN_SENDDATA_ERROR){
        return 0;
    }
    PCCONN_Conn_2_Conn(conn,conn2,usectime);
    return 1;
}

int CC_onLcxTran(pPCConn conn){
    int bufferlen = (sizeof(char)*MAX_IP_ADDR_LEN + sizeof(int));
    char *recvbuffer = (char *)malloc(bufferlen);
    int res = PCCONN_RecvData(conn,recvbuffer,bufferlen);
    if(res == PCCONN_RECVDATA_ERROR){
        return 0;
    }
    char rip[MAX_IP_ADDR_LEN];
    int  rport;
    memcpy(rip,recvbuffer,MAX_IP_ADDR_LEN);
    rport = API_m_chartoi(&(recvbuffer[MAX_IP_ADDR_LEN]),4);
    pPCConn conn2 = PCCONN_Connect(rip,rport);
    if(conn2 == PCCONN_CONNECT_ERROR){
        return 0;
    }
    PCCONN_Conn_2_Conn(conn,conn2,10000);
    return 1;
}

int CC_Lcx_Tran(int targetid,int lport,char *rip,int rport,int usec){
    if(targetid < 0 || lport <=0 || strlen(rip)>MAX_IP_ADDR_LEN ||
        rport <= 0){
        return CC_LCX_TRAN_ERROR;
    }
    pLcxTranStruct pvalue = (pLcxTranStruct)
        malloc(sizeof(LcxTranStruct));
    if(pvalue == NULL){
        return CC_LCX_TRAN_ERROR;
    }
    pvalue ->targetid = targetid;
    pvalue -> rport   = rport;
    strncpy(pvalue->rip,rip,MAX_IP_ADDR_LEN);
    pvalue -> usec    = usec;
    int res = PCCONN_Listen(lport,
        20,
        fun_callback_for_StartLcxTran,
        (char *)pvalue);
    if(res == PCCONN_LISTEN_ERROR){
        return CC_LCX_TRAN_ERROR;
    }
    return CC_LCX_TRAN_OK;
}

