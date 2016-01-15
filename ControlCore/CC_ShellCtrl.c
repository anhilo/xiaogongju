/**
* File Name :ControlCore/CC_ShellCtrl.c
* Author: rootkiter
* E-mail: rootkiter@rootkiter.com
* Created Time : 四  1/14 22:59:56 2016
*/
#include "CC_ShellCtrl.h"
#include "../AgentManager/PCConn.h"
#include "../AgentManager/AgentConversationCTRL.h"

int fun_callback_for_StartShell(pPCConn conn,char *value){
    int targetid = API_m_chartoi(value,4);
    Printf_DEBUG("START shell target id is %d",
        targetid);
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

int CC_onStartShellhere(pPCConn conn);
