#include "PCNodeManager.h"

//**********************************************
int fun_pcInfo_free(pNodeData node){
    PCNODE_Free((pPCNodeInfo)node);
    return 1;
}

pNodeData fun_pcInfo_copy(pNodeData node){
    return (pNodeData)PCNODE_Copy((pPCNodeInfo)node);
}
//**********************************************
pTreeManager tree;
pLISTHEAD    list;
int root_id = 0;
#define MANAGER_NOW 1
#define NO_MANAGER  2
#define IAM_ADMIN   3
int tree_now= NO_MANAGER;
pPCNodeInfo rootnode;
int fatherid = -1;
int maxid = -1;
//**********************************************
int MANAGER_FREE(){
    if(list != NULL){ free(list) ;}
    if(tree != NULL){ free(tree) ;}
    return 1;
}

int PCMANAGER_INIT(pPCNodeInfo nodeself){
    rootnode = PCNODE_Copy(nodeself);
    if(rootnode -> NodeType == 
        IAM_ADMIN_NODE){
Printf_DEBUG("cccccccccccccccccccccc");
        tree_now = IAM_ADMIN;
    }
Printf_DEBUG("root->nodetype = %d . %d",
        rootnode->NodeType , IAM_ADMIN_NODE);
    if(rootnode == NULL) return PCMANAGER_INIT_ERROR;
    tree = Tree_Init_Manager(NULL,
        fun_pcInfo_free,
        fun_pcInfo_copy);
    list = ListNode_BuildListHead(NULL,
        fun_pcInfo_free,
        fun_pcInfo_copy);
    if(tree == NULL || list == NULL){
        MANAGER_FREE();
        return PCMANAGER_INIT_ERROR;
    }
// remember add result check
    Tree_BuildTreeRoot(tree,root_id,rootnode);
// remember add result check
    root_id = rootnode -> id;
    tree_now = NO_MANAGER;
    maxid = rootnode->id;
    if(rootnode->NodeType == 
        IAM_ADMIN_NODE){
        tree_now = IAM_ADMIN;
    }
    return PCMANAGER_INIT_OK;
}

int PCMANAGER_ADDNeighbor(pPCNodeInfo newnode){
    if(PCMANAGER_HAVENode(newnode->id) 
        == PCMANAGER_HAVANODE_YES){
Printf_DEBUG("ID CLASH Here");
        return PCMANAGER_ADDNEIGHBOR_ID_CLASH;
    }
    int res = ListNode_InsertNode(
                list,
                newnode->id,
                (pNodeData)newnode);
    if(res == LISTNODE_INSERTNODE_ERROR){
        return PCMANAGER_ADDNEIGHBOR_ERROR;
    }
    if(tree_now == MANAGER_NOW
        || tree_now == IAM_ADMIN ){  // no tree
        int res2 = Tree_InsertNode(tree,
                    root_id,
                    newnode->id,
                    (pNodeData)newnode);
        if(res2 == TREE_INSERTNODE_ERROR){
            return PCMANAGER_ADDNEIGHBOR_ERROR;
        }
    }
    maxid = (maxid>newnode->id)?maxid:newnode->id;
    return PCMANAGER_ADDNEIGHBOR_OK;
}

int PCMANAGER_ADDRemote(int fatherid,pPCNodeInfo newnode){
    if(tree_now == NO_MANAGER 
         ){  // no tree
    Printf_DEBUG("Not Manager or admin");
        return PCMANAGER_ADDREMOTE_ERROR;
    }
    int res = Tree_InsertNode(tree,
                fatherid,
                newnode->id,
                (pNodeData)newnode);
    if(res == TREE_INSERTNODE_ERROR){
    Printf_DEBUG("remote insert error");
        return PCMANAGER_ADDREMOTE_ERROR;
    }
    maxid = (maxid>newnode->id)?maxid:newnode->id;
    return PCMANAGER_ADDREMOTE_OK;
}

int funcallback_ForSetUpper(pNodeData node){
    pPCNodeInfo info = (pPCNodeInfo)node;
    if(info == NULL){  return 0;}
    if(info->NodeType == UPSTREAM_NODE){
 //       PCNODE_Free(info);
        return 1;
    }
    int res = Tree_InsertNode(tree,
                root_id,
                info->id,
                node);
    if(res == TREE_INSERTNODE_ERROR){
//        PCNODE_Free(info);
        return 0;
    }
 //   PCNODE_Free(info);
    return 1;
}


int PCMANAGER_SETUpperAdmin(int nodeid){
    if(tree_now == MANAGER_NOW)
         return PCMANAGER_SETUPPER_ERROR;
    pPCNodeInfo node = 
        (pPCNodeInfo) ListNode_GetNodeData(list,nodeid);
    if( node == NULL){
        return PCMANAGER_SETUPPER_ERROR;
    }
    node->NodeType = UPSTREAM_NODE;
    int res = ListNode_ReplaceNodeData(list,nodeid,node);
    if(res == LISTNODE_REPLACENODEDATA_ERROR){
        return PCMANAGER_SETUPPER_ERROR;
    }
    res = ListNode_Traversal(list,
                funcallback_ForSetUpper);
    if(LISTNODE_TRAVERSAL_ERROR == res){
        return PCMANAGER_SETUPPER_ERROR;
    }
    tree_now = MANAGER_NOW;
    fatherid = nodeid ;
    PCNODE_Free(node);
    node = NULL;
    return PCMANAGER_SETUPPER_OK;
}

int PCMANAGER_UpperAdminLost(){
// remember add result check here
    Tree_CleanUp(tree);
    Tree_BuildTreeRoot(tree,root_id,rootnode);
// please add result check here
    tree_now = NO_MANAGER;
    fatherid = -1;
    return PCMANAGER_UPPERADMINLOST_OK;
}

int funcallback_For_showMap(pNodeData node){
    pPCNodeInfo info = (pPCNodeInfo)node;
    if(info == NULL){ return 0; }
    printf("{id:%d,OSType:%d,pcname:%s,linktype:%d}\n",
        info -> id,
        info -> OSType,
        info -> PCName,
        info ->NodeType);
    info = NULL;
    return 1;
}
    
int PCMANAGER_ShowMap(){
    printf("***************1 Neighbor node list ************\n");
    ListNode_Traversal(list,
        funcallback_For_showMap);
    if(tree_now == NO_MANAGER) return PCMANAGER_SHOWMAP_OK;
    printf("*************** Tree node map      ************\n");
    Tree_Traversal(tree,
        funcallback_For_showMap);
    return PCMANAGER_SHOWMAP_OK;
}


int PCMANAGER_RemoveNode(int id){
    int res = -1;
    pPCNodeInfo node = 
        (pPCNodeInfo) ListNode_GetNodeData(list,id);
    if(node != NULL){
        res = ListNode_DeleteNode(list,id);
        if(res == LISTNODE_DELETENODE_ERROR){
            return PCMANAGER_REMOVENODE_ERROR;
        }
        PCNODE_Free(node);
        node = NULL;
    }
    node = (pPCNodeInfo) Tree_GetNodeData(tree,id);
    if(node != NULL){
        res = Tree_RemoveNode(tree,id);
        if(res == TREE_REMOVENODE_ERROR){
            return PCMANAGER_REMOVENODE_ERROR;
        }
        PCNODE_Free(node);
        node = NULL;
    }
    return PCMANAGER_REMOVENODE_OK;
}

int PCMANAGER_ReplaceID(int oldid,int newid){
    int res = -1;
    int res2 = -1;
    pPCNodeInfo node = 
        (pPCNodeInfo) ListNode_GetNodeData(list,oldid);
    if(node != NULL){
        res = ListNode_Replaceid(list,oldid,newid);
        node->id = newid;
        res2 = ListNode_ReplaceNodeData(list,
                    newid,
                    (pNodeData)node);
        if(res == LISTNODE_REPLACEID_ERROR
            || res2 == LISTNODE_REPLACENODEDATA_ERROR){
            return PCMANAGER_REPLACEID_ERROR;
        }
        PCNODE_Free(node);
        node = NULL;
    }
    node = (pPCNodeInfo) Tree_GetNodeData(tree,oldid);
    if(node != NULL){
        res = Tree_Replaceid(tree,oldid,newid);
        node->id = newid;
        res2 = Tree_ReplaceNode(tree,newid,
                (pNodeData)node);
        if(res == TREE_REPLACEID_ERROR
            || res2 == TREE_REPLACENODE_ERROR){
            return PCMANAGER_REPLACEID_ERROR;
        }
        PCNODE_Free(node);
        node = NULL;
    }
    maxid = (maxid>newid)?maxid:newid;
    if(oldid == rootnode->id){
        rootnode->id = newid;
    }
    return PCMANAGER_REPLACEID_OK;
}

pPCNodeInfo PCMANAGER_GETNodeInfo(int id){
    int res = -1;
    pPCNodeInfo node = 
        (pPCNodeInfo) ListNode_GetNodeData(list,id);
Printf_DEBUG("bbbbbbbbbbbbbbbbb");
    if(node == NULL){
Printf_DEBUG("ccccccccccccccccc");
        node = (pPCNodeInfo)
            Tree_GetNodeData(tree,id);
    }
    if(node == NULL){
        return PCMANAGER_GETNODEINFO_ERROR;
    }
    return node;
}

pPCNodeInfo PCMANAGER_GETNextJump(int id){
    int res = -1;
    int nextid = -1;
    pPCNodeInfo node = 
        (pPCNodeInfo) ListNode_GetNodeData(list,id);
    if(node == NULL){
        nextid = Tree_GetNextJump(tree,id);
        if(nextid == -1){
            return PCMANAGER_GETNEXTJUMP_ERROR;
        }
        node = (pPCNodeInfo)ListNode_GetNodeData(list,nextid);
    }
    if(node == NULL){
        return PCMANAGER_GETNEXTJUMP_ERROR;
    }
    return node;
}


int PCMANAGER_GETNeighborNum(){
    return ListNode_GetListNum(list);
}

int PCMANAGER_GETAllNodeNum(){
    return Tree_GetNodesNum(tree);
}

int PCMANAGER_HAVENode(int id){
    if( ListNode_HaveNode(list,id)
        || Tree_HaveNode(tree,id)){
        return PCMANAGER_HAVANODE_YES;
    }
    return PCMANAGER_HAVANODE_NO;
        
}

int PCMANAGER_Traversal_Neighbor(pfunNodeCall callback){
    int result = 0;
    if(list == NULL || callback == NULL){
        return PCMANAGER_TRAVERSAL_NEIGHBOR_ERROR;
    }
    result = ListNode_Traversal(list,callback);
    if(result == LISTNODE_TRAVERSAL_ERROR){
        return PCMANAGER_TRAVERSAL_NEIGHBOR_ERROR;
    }
    return PCMANAGER_TRAVERSAL_NEIGHBOR_OK;
}

int PCMANAGER_Get_Fresh_ID(){
    while(
        maxid >= 0
        &&
        PCMANAGER_HAVANODE_YES == 
        PCMANAGER_HAVENode(maxid)
    ){
        maxid ++;
    }
    if(maxid >=0 ){
//Printf_DEBUG("maxid is %d.",maxid);
Printf_DEBUG("Fresh new id %d ",maxid);
        return maxid;
    }
    return -1;
}

pPCNodeInfo PCMANAGER_Get_RootNode(){
    return  PCNODE_Copy(rootnode);
}

int PCMANAGER_Get_RootID(){
    return rootnode->id;
}

pPCNodeInfo PCMANAGER_Get_FatherNode(){
    if(tree_now != MANAGER_NOW){
        return PCMANAGER_GET_FATHERNODE_ERROR;
    }
    pPCNodeInfo info = PCMANAGER_GETNodeInfo(fatherid);
    if( info == PCMANAGER_GETNODEINFO_ERROR ){
        return PCMANAGER_GET_FATHERNODE_ERROR;
    }
    return info;
}

int PCMANAGER_Manager_Now(){
    if(tree_now == MANAGER_NOW){
        return PCMANAGER_MANAGER_NOW_TRUE;
    }
    return PCMANAGER_MANAGER_NOW_FALSE;
}
