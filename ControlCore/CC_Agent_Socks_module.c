/**
* File Name :ControlCore/CC_Agent_Socks_module.c
* Author: rootkiter
* E-mail: rootkiter@rootkiter.com
* Created Time : 二  1/19 00:16:18 2016
*/
#include "CC_Agent_Socks_module.h"
#include "Cmd_Ctrl.h"
#include "../sockslib/SocksBase.h"

typedef struct socksValue{
    int targetid;
    int usec;
}SOCKSDATA,*pSOCKSDATA;


int fun_callback_for_StartSocks(pPCConn conn,char *value){
    pSOCKSDATA pvalue = (pSOCKSDATA)value;
    if(pvalue == NULL || conn == NULL){
        return 0;
    }
    int targetid = pvalue -> targetid;
    int usec     = pvalue -> usec ;
    pPCConn conn2 = CMDCTRL_BuildTargetSock(
            targetid,
            AGENT_SERVER_COMMAND_SOCKS
    );
    if( CMDCTRL_BUILDTARGETSOCK_ERROR == conn2){
        return 0;
    }
    char usecbuf[4];
    API_m_itochar(usec,usecbuf,4);
    PCCONN_SendData(conn2,usecbuf,4);
    PCCONN_Conn_2_Conn(conn,conn2,usec);
    return 1;
}

int CC_StartSocks(int targetid,int lport,int usec){
    if(targetid < 0 || lport <=0 || usec <= 0){
        return CC_STARTSOCKS_ERROR;
    }
    pSOCKSDATA pvalue = (pSOCKSDATA)malloc(sizeof(SOCKSDATA));
    if(pvalue == NULL){
        return CC_STARTSOCKS_ERROR;
    }
    pvalue -> targetid = targetid;
    pvalue -> usec     = usec;
    int res = PCCONN_Listen(
        lport,
        200,
        fun_callback_for_StartSocks,
        (char*)pvalue
    );
    if( PCCONN_LISTEN_ERROR ==  res){
        return CC_STARTSOCKS_ERROR;
    }
    return CC_STARTSOCKS_OK;
}


int on_CC_startSocks(pPCConn conn){
    if(conn == NULL){
        return 0;
    }
    int sock = conn -> cmd_socket;

    char usecbuf[4];
    PCCONN_RecvData(conn,usecbuf,4);
    int usec = API_m_chartoi(usecbuf,4);
    int targetsock = socks_build_target_socket(sock);
    tunn_sock_to_sock(targetsock,sock,usec);
    return 1;
}
