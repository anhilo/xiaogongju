/**
* File Name :AgentManager/AgentChildSync.c
* Author: rootkiter
* E-mail: rootkiter@rootkiter.com
* Created Time : Tue 17 Nov 2015 01:01:20 AM PST
*/

#include "AgentChildSync.h"



int m_ForEachChildAgent_Trigger(pNodeData minfo){
    pPCNodeInfo info = (pPCNodeInfo)minfo;
    int newid = ASK_NEW_ID();
    // set it id
    resetTargetNewId(info->id,newid);
    // send set id info
    PCMANAGER_ReplaceID(info->id,newid);
    info->id = newid;
    // send agent info upper
    SendAgentInfo(PCMANAGER_Get_RootID(),
        newid,
        info->OSType,
        info->PCName);
    return 1;
}

int ChildNodeInfoSyncTrigger(){
    // Traval every node 
    PCMANAGER_Traversal_Neighbor();
    // and call m_ForEachChildAgent_Trigger
}

int on_ChildNodeInfo_Arrive(int sock){

}
