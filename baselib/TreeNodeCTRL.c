#include"TreeNodeCTRL.h"

pTreeManager Tree_Init_Manager(
    pfunNodeCall  fun_call,
    pfunNodeFree  fun_free, 
    pfunNodeCopy  fun_copy
){
    pTreeManager head = (pTreeManager)malloc(sizeof(TreeManager));
    //head -> node_num  = 0;
    head -> root = NULL;
    head -> fun_call  = fun_call;
    head -> fun_free  = fun_free;
    head -> fun_copy  = fun_copy;
    head -> child_num = 0;
    return head;
}

int Tree_BuildTreeRoot(pTreeManager head,int id,pNodeData node){
    pTreeNode p = NULL;
    if(head -> root != NULL){
        printf("remember free this tree");
        return 0;
    }
    p = (pTreeNode)malloc(sizeof(TreeNode));
    p -> id              = id;
    p -> node            = head->fun_copy(node);
    p -> father          = NULL;
    p -> child_num       = 0;
    p -> child           = NULL;
    p -> pre_Sibling     = p;
    p -> next_Sibling    = p;
    
    head -> root = p;
    return 1;
}

pTreeNode mGetNode(pTreeNode root,int id){
    int k = 0;
    TreeNode *p = NULL;
    TreeNode *result = NULL;
    if(root==NULL){
        return NULL;
    }
    else if(root->id == id){
        return root;
    }
    else if(root->child_num > 0){
        p = root->child;
        for(k=0;k< root->child_num ; k++){
            result =  mGetNode(p,id);
            if (result != NULL){
                return result;
            }
            p = p->next_Sibling;
            if( p == root->child){
                // end link list
                return NULL;
            }
        }
    }
    return NULL;
}


pTreeNode GetNode(pTreeManager head,int id){
    if(head == NULL){
        return NULL;
    }
    return mGetNode(head->root,id);
}

int Tree_InsertNode(pTreeManager head,int parrentid,int newid, pNodeData pnode){
    TreeNode *node = GetNode(head,parrentid);
    if(node ==NULL){
        return TREE_INSERTNODE_ERROR;
    }
    TreeNode *newNode = (TreeNode*)malloc(sizeof(TreeNode));
    if(newNode ==NULL){
        node = NULL;
        return TREE_INSERTNODE_ERROR;
    }
    // init new_node value
    newNode->id   = newid;
    newNode->node = head->fun_copy(pnode);
    newNode->child_num = 0;
    newNode->child = NULL;
    // set Sibling link
    if( node ->child_num == 0 ){
        if( node ->child != NULL){
            free(newNode);
            node = NULL;
            return TREE_INSERTNODE_ERROR;
        }
        newNode -> pre_Sibling  = newNode;
        newNode -> next_Sibling = newNode;
    }
    else{
        newNode->pre_Sibling = node -> child -> pre_Sibling;
        newNode->next_Sibling= node -> child ;

        node-> child  -> pre_Sibling -> next_Sibling = newNode;
        node-> child -> pre_Sibling  = newNode;
    }
    //  reset father node state
    newNode->father = node;
    node-> child = newNode;
    node-> child_num ++;
    
    head -> child_num ++;
    return TREE_INSERTNODE_OK;
}

int Free(pTreeManager head,pTreeNode node){
    int k;
    pTreeNode buff = NULL,p;
    if(node == NULL){
        // NULL node ? kidding me ?
        return 0;
    }
    // free children  nodes
    p = node -> child;
    for(k = 0;k < node -> child_num ;k++){
        int res = Free(head,p);
        printf("node_id = %d ,child_num = %d,k = %d\n",
            node->id,node->child_num,k);
        if(res == 0){  
            printf("res = 0? child_num = %d, k = %d\n",
                node-> child_num,k);
            return 0 ;
        }
        p = p-> next_Sibling;
    }
    if(node -> next_Sibling != node){
        // remove node from link list
        buff = node -> next_Sibling;
        node -> pre_Sibling  -> next_Sibling = node -> next_Sibling;
        node -> next_Sibling -> pre_Sibling  = node -> pre_Sibling;
    }
    // init some values
    node -> child = NULL;
    node -> child_num = 0;
    // free node-> node
    head -> fun_free(node -> node);
    // free it self
    free (node);
    head -> child_num --;
    return 1;
}

int Tree_RemoveNode(pTreeManager head,int removeid){
    TreeNode *root = head -> root;
    TreeNode *node = GetNode(head,removeid);
    TreeNode *buff = NULL;
    if( node ==NULL  ){
        return TREE_REMOVENODE_ERROR;
    }
    buff = node -> next_Sibling;
    if(node -> father != NULL){
        // remove node from father node
        if(node -> father -> child == node){
            node -> child = buff;
            buff = NULL;
        }
        node -> father -> child_num --;
    }
    Free(head,node);
    return TREE_REMOVENODE_OK;
}

int Tree_Replaceid(pTreeManager head,int oldid,int newid){
    TreeNode *root = head -> root;
    TreeNode *p = GetNode(head,oldid);
    if(p == NULL){
        return TREE_REPLACEID_ERROR;
    }
    else{
        p->id = newid;
        return TREE_REPLACEID_OK;
    }
}

int Tree_ReplaceNode(pTreeManager head,int id,pNodeData node){
    TreeNode *root = head -> root;
    TreeNode *p    = GetNode(head,id);
    if(p == NULL){
        return TREE_REPLACENODE_ERROR;
    }
    else{
        head->fun_free(p->node);
        p->node = head->fun_copy(node);
        return TREE_REPLACENODE_OK;
    }
}

pNodeData Tree_GetNodeData(pTreeManager head,int id){
    TreeNode *root = head -> root;
    TreeNode *p = GetNode(head,id);
    if(p != NULL){
        return head->fun_copy(p->node);
    }
    return NULL;
}

int mTraversal(TreeNode *root , pfunNodeCall callback){
    int k;
    TreeNode *p = NULL;
    if(root ==NULL){
        return 0;
    }
//    printf("this id is %d , child_num = %d\n",root->id, root->child_num);
    callback(root->node);
    p = root->child;
    for(k=0;k<root->child_num;k++){
//printf(">>>>start print child this id = %d \n",root->id);
        mTraversal(p,callback);
        p = p->next_Sibling;
    }
    return 1;
}

int Tree_Traversal(pTreeManager head,pfunNodeCall callback){
    TreeNode *p = head ->root;
    int k;
    if(p ==NULL){
        printf("Zero tree");
        return 0;
    }
    mTraversal(p,callback);
    return 1;
}

int Tree_GetNextJump(pTreeManager head,int id){
    TreeNode *root = head -> root;
    int k = 0;
    TreeNode *p = NULL;
    TreeNode * result = NULL;
    if(root==NULL){
        return -1;
    }
    else if(root->id == id){
        return root->id;
    }
    else if(root->child_num > 0){
        p = root->child;
        for(k=0;k< root->child_num ; k++){
            result =  GetNode(head,id);
            if (result != NULL){
                return p->id;
            }
            p = p->next_Sibling;
            if( p == root->child){
                // end link list
                return 0;
            }
        }
    }
    return 0;
}

int Tree_GetNodesNum(pTreeManager head){
    return head -> child_num;
}

int Tree_CleanUp(pTreeManager head){
    if(head->child_num < 0 
        || head -> root != NULL){
        return TREE_CLEANUP_ERROR;
    }
    Tree_RemoveNode(head,head->root->id);
    head->child_num = 0;
    head->root = NULL;
    return TREE_CLEANUP_OK;
}
