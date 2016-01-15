/**
* File Name :ControlCore/CC_ShellCtrl.c
* Author: rootkiter
* E-mail: rootkiter@rootkiter.com
* Created Time : 四  1/14 22:59:56 2016
*/
#include "CC_ShellCtrl.h"
#include "../AgentManager/PCConn.h"
#include "../AgentManager/AgentConversationCTRL.h"
#include "CCProxy.h"
#include "Cmd_Ctrl.h"

int fun_callback_for_StartShell(pPCConn conn,char *value){
    int targetid = API_m_chartoi(value,4);
    Printf_DEBUG("START shell target id is %d",
        targetid);

    pPCConn conn2 = CMDCTRL_BuildTargetSock(targetid,
        AGENT_SERVER_COMMAND_CMDSHELL);
    if( CMDCTRL_BUILDTARGETSOCK_ERROR == conn2 ){
        Printf_DEBUG("SendMSG Build target tunnel Error targetid = %d",targetid);
        return 0;
    }
    PCCONN_Conn_2_Conn(conn,conn2,100000);
    return 1;
}


int CC_StartShell(int targetid,int locolport){
    char *targetarg = (char *)malloc(sizeof(int));
    API_m_itochar(targetid,targetarg,4);
    int res = PCCONN_Listen(locolport,
            20,
            fun_callback_for_StartShell,
            targetarg);
    if(res == PCCONN_LISTEN_ERROR){
        return CC_STARTSHELL_ERROR;
    }
    return CC_STARTSHELL_OK;
}

int CC_onStartShellhere(pPCConn conn){
    int sock = conn->cmd_socket;
    API_Start_ShellThread_for_sock(sock);
    return 1;
}
