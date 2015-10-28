#ifndef _LISTNODECTRL_H_
#define _LISTNODECTRL_H_

#include"../generic.h"

typedef struct ListNode{
    int id;
    pNodeData node;
    struct ListNode *pre_Node;
    struct ListNode *next_Node;
}LISTNODE,*pLISTNODE;

typedef struct ListNodeHead{
    pLISTNODE head;
    int nodenum;
    pfunNodeCall  fun_call;
    pfunNodeFree  fun_free;
    pfunNodeCopy  fun_copy;
}LISTHEAD,*pLISTHEAD;


pLISTHEAD ListNode_BuildListHead(
    pfunNodeCall  fun_call,
    pfunNodeFree  fun_free, 
    pfunNodeCopy  fun_copy
);

#define LISTNODE_INSERTNODE_OK     1
#define LISTNODE_INSERTNODE_ERROR -1
int ListNode_InsertNode(pLISTHEAD head,int id,pNodeData newdata);

#define LISTNODE_DELETENODE_ERROR  -1
#define LISTNODE_DELETENODE_OK   1
int ListNode_DeleteNode(pLISTHEAD head,int nodeid);

int ListNode_GetListNum(pLISTHEAD head);

#define LISTNODE_REPLACEID_ERROR   -1
#define LISTNODE_REPLACEID_OK       1
int ListNode_Replaceid(pLISTHEAD head,int oldid,int newid);

#define LISTNODE_REPLACENODEDATA_ERROR -1
#define LISTNODE_REPLACENODEDATA_OK     1
int ListNode_ReplaceNodeData(pLISTHEAD head , int id,pNodeData newdata);

pNodeData ListNode_GetNodeData(pLISTHEAD head,int id);

#define LISTNODE_TRAVERSAL_ERROR      -1
#define LISTNODE_TRAVERSAL_OK          1
int ListNode_Traversal(pLISTHEAD head,pfunNodeCall callback);

int ListNode_VisitNode(pLISTHEAD head,int id,pfunNodeCall callback);
#endif
