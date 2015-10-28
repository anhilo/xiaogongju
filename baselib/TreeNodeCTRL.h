#ifndef _TREENODECONN_H_
#define _TREENODECONN_H_ 

#include"../generic.h"

typedef struct MyTreeNode{
    int id;
    pNodeData node;
    struct MyTreeNode *father;
    int    child_num;
    struct MyTreeNode *child;
    struct MyTreeNode *pre_Sibling;
    struct MyTreeNode *next_Sibling;
}TreeNode,*pTreeNode;


typedef struct TreeManage{
    pTreeNode root;
    int child_num;
    pfunNodeCall  fun_call;
    pfunNodeFree  fun_free;
    pfunNodeCopy  fun_copy;
}TreeManager,*pTreeManager;

pTreeManager Tree_Init_Manager(
    pfunNodeCall  fun_call,
    pfunNodeFree  fun_free, 
    pfunNodeCopy  fun_copy
);

int Tree_BuildTreeRoot(pTreeManager head,int id,pNodeData nodeself);

//int Tree_SetRootParrent(int parrentid,pNodeData parrentnode);
#define TREE_INSERTNODE_ERROR -1
#define TREE_INSERTNODE_OK     1
int Tree_InsertNode(pTreeManager head,int parrentid,int newid, pNodeData pnode);

//////  A Bug here  we need remove node and its children
#define TREE_REMOVENODE_ERROR -1
#define TREE_REMOVENODE_OK     1
int Tree_RemoveNode(pTreeManager head,int removeid);

#define TREE_REPLACEID_ERROR -1
#define TREE_REPLACEID_OK     1
int Tree_Replaceid(pTreeManager head,int oldid,int newid);

#define TREE_REPLACENODE_ERROR -1
#define TREE_REPLACENODE_OK     1
int Tree_ReplaceNode(pTreeManager head,int id,pNodeData node);

#define TREE_GETNODEDATA_ERROR -1
pNodeData Tree_GetNodeData(pTreeManager head,int id);

//int Tree_Free(TreeNode *root,CallBackForNodeData dataFreeAddr);

//int Tree_Print(TreeNode *root);

int Tree_GetNextJump(pTreeManager head,int id);

int Tree_Traversal(pTreeManager head ,pfunNodeCall callback);

int Tree_GetNodesNum(pTreeManager head);

#define TREE_CLEANUP_ERROR  -1
#define TREE_CLEANUP_OK      1
int Tree_CleanUp(pTreeManager head);
#endif
