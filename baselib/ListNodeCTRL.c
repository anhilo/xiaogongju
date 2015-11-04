#include "ListNodeCTRL.h"

pLISTHEAD ListNode_BuildListHead(
    pfunNodeCall  fun_call,
    pfunNodeFree  fun_free, 
    pfunNodeCopy  fun_copy
){
    pLISTHEAD head    = (pLISTHEAD)malloc(sizeof(LISTHEAD)); 
    head->nodenum     = 0;
    head->head        = NULL;
    head->fun_call    = fun_call;
    head->fun_free    = fun_free;
    head->fun_copy    = fun_copy;
    return head;
}


int ListNode_InsertNode(pLISTHEAD head,int id,pNodeData newdata){
    pLISTNODE p = (pLISTNODE)malloc(sizeof(LISTNODE));
    if(p==NULL)
    {
        return LISTNODE_INSERTNODE_ERROR;
    }
    //  set node data
    p->id        = id;
    p->node      = head->fun_copy(newdata);
    //  pre_Node always NULL
    p->pre_Node  = NULL;
    //  next_Node always head->head
    p->next_Node = head->head;

    //  if list not NULL ,need set the pre_Node point
    if(head->head != NULL){
        head->head->pre_Node = p;
    }
    //  insert new node to the list
    head->head = p;
    //  add the num
    head->nodenum ++;
    return LISTNODE_INSERTNODE_OK;
}

pLISTNODE GetLISTNODE(pLISTHEAD head,int nodeid){
    pLISTNODE p = head -> head;
    int k = 0;
    while(k<head->nodenum && p != NULL){
        if(nodeid == p->id){
            return p;
        }
        p = p->next_Node;
    }
    return NULL;
}

int ListNode_DeleteNode(pLISTHEAD head,int nodeid){
    pLISTNODE p = GetLISTNODE(head,nodeid);
    if(p == NULL){
        return LISTNODE_DELETENODE_ERROR;
    }
    // remove p from the list
    if(p->pre_Node !=NULL){
        p->pre_Node->next_Node = p->next_Node;
    }
    if(p->next_Node !=NULL){
        p->next_Node->pre_Node = p->pre_Node;
    }
    if( head->head == p){
        // if it is the first node
        head->head = p->next_Node;
    }
    p->pre_Node =  NULL;
    p->next_Node = NULL;
    head->nodenum --;
    head->fun_free(p->node);
    free(p);
    p = NULL;
    return LISTNODE_DELETENODE_OK;
}

int ListNode_GetListNum(pLISTHEAD head){
    return head->nodenum;
}

int ListNode_Replaceid(pLISTHEAD head,int oldid,int newid){
    pLISTNODE p = GetLISTNODE(head,oldid);
    if(p==NULL){
        return LISTNODE_REPLACEID_ERROR;
    }
    p->id = newid;
    return LISTNODE_REPLACEID_OK;
}

int ListNode_ReplaceNodeData(pLISTHEAD head , int id,pNodeData newdata){
    pLISTNODE p = GetLISTNODE(head,id);
    if(p==NULL){
        return LISTNODE_REPLACENODEDATA_ERROR;
    }
    head->fun_free(p->node);
    p->node = head->fun_copy(newdata);
    return LISTNODE_REPLACENODEDATA_OK;
}

pNodeData ListNode_GetNodeData(pLISTHEAD head,int id){
    pLISTNODE p = GetLISTNODE(head,id);
    if(p==NULL){
        return NULL;
    }
    return head->fun_copy(p->node);
}

int ListNode_Traversal(pLISTHEAD head,pfunNodeCall callback){
    pLISTNODE p = head->head;
    pNodeData node = NULL;
    int k;
    for(k=0;k<head->nodenum;k++){
        node = head->fun_copy(p->node);
        callback(node);
        head->fun_free(node);
        p = p->next_Node;
    }
    return 1;
}

int ListNode_VisitNode(pLISTHEAD head,int id,pfunNodeCall callback){
    pLISTNODE p = GetLISTNODE(head,id);
    if(p==NULL){
        return 0;
    }
    callback(p->node);
    return 1;
}

int ListNode_HaveNode(pLISTHEAD head, int id){
    pLISTNODE p = GetLISTNODE(head,id);
    if(p == NULL){
        return 0;
    }
    return 1;
}
