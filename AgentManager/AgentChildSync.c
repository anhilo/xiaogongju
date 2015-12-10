/**
* File Name :AgentManager/AgentChildSync.c
* Author: rootkiter
* E-mail: rootkiter@rootkiter.com
* Created Time : Tue 17 Nov 2015 01:01:20 AM PST
*/

#include "AgentChildSync.h"
#include "PCNodeInfo.h"
#include "AgentMsgHandle.h"
#include "PCNodeManager.h"

int m_ForEachChildAgent_Trigger(pNodeData minfo){
    pPCNodeInfo info = (pPCNodeInfo)minfo;
    if(info ->NodeType == 
        MYSELF_NODE_REMOTE
      || info -> NodeType == UPSTREAM_NODE){
        return 1;
    }
    int newid = ASK_NEW_ID();
    // set it id
    resetTargetNewId(info->id,newid);
    // send set id info
//    PCMANAGER_ReplaceID(info->id,newid);
    int oldid = info -> id;
    info->NodeType = MYSELF_NODE_REMOTE;
    info->id = newid;
    /// set that node remote
PCMANAGER_Tree_Print();
Printf_DEBUG("oldid = %d,,,,newid = %d,rootid = %d",
        oldid,newid,PCMANAGER_Get_RootID());
//    PCMANAGER_RemoveNode(oldid);
//    PCMANAGER_ADDNeighbor(info);
    PCMANAGER_ReplaceID(oldid,newid);
Printf_DEBUG("oldid = %d,,,,newid = %d,rootid = %d",
        oldid,newid,PCMANAGER_Get_RootID());
Printf_DEBUG("Add OK");
    // send agent info upper
    SendAgentInfo(PCMANAGER_Get_RootID(),
        newid,
        info->OSType,
        info->PCName);
    return 1;
}

int ChildNodeInfoSyncTrigger(){
    // Traval every node 
    // and call m_ForEachChildAgent_Trigger
    PCMANAGER_Traversal_Neighbor(
        m_ForEachChildAgent_Trigger);
    return 1;
}

