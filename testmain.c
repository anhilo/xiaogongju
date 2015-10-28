#include "generic.h"
#include "AgentManager/PCNodeInfo.h"
#include "AgentManager/PCNodeManager.h"

int main(){
    printf("justtest\n");
// set rootnode
    pPCNodeInfo rootnode = PCNODE_Create();
    PCNODE_SETAllData(rootnode,
        0,
        PC_OS_LINUX,
        "This is a linux OS,rootnode",
        NEIGHBOR_NODE,
        "127.0.0.1",
        -1,
        -1);
    PCMANAGER_INIT(rootnode);
// add neighbor node
    pPCNodeInfo nei_node1 = PCNODE_Create();
    PCNODE_SETAllData(nei_node1,
        1,
        PC_OS_WINDOW,
        "Windows,Neighbor node1",
        NEIGHBOR_NODE,
        "192.168.1.1",
        8888,
        -1);
    PCMANAGER_ADDNeighbor(nei_node1);

    pPCNodeInfo nei_node2 = PCNODE_Create();
    PCNODE_SETAllData(nei_node2,
        2,
        PC_OS_MACOSX,
        "MACOS,Neighbor node2",
        NEIGHBOR_NODE,
        "192.168.1.1",
        8888,
        -1);
    PCMANAGER_ADDNeighbor(nei_node2);

    pPCNodeInfo nei_node3 = PCNODE_Create();
    PCNODE_SETAllData(nei_node3,
        3,
        PC_OS_ARMLINUX,
        "ARMLinux,Neighbor node3",
        NEIGHBOR_NODE,
        "192.168.1.1",
        8888,
        -1);
    PCMANAGER_ADDNeighbor(nei_node3);

    pPCNodeInfo nei_node4 = PCNODE_Create();
    PCNODE_SETAllData(nei_node4,
        4,
        PC_OS_LINUX,
        "Linux,Neighbor node4",
        NEIGHBOR_NODE,
        "192.168.1.1",
        8888,
        -1);
    PCMANAGER_ADDNeighbor(nei_node4);

    PCMANAGER_SETUpperAdmin(2);
// add remote node
    pPCNodeInfo nei_node5 = PCNODE_Create();
    PCNODE_SETAllData(nei_node5,
        5,
        PC_OS_LINUX,
        "Linux,Neighbor node5",
        NEIGHBOR_NODE,
        "192.168.1.1",
        8888,
        -1);
    PCMANAGER_ADDRemote(3,nei_node5);
    
    pPCNodeInfo nei_node6 = PCNODE_Create();
    PCNODE_SETAllData(nei_node6,
        6,
        PC_OS_LINUX,
        "Linux,Neighbor node6",
        NEIGHBOR_NODE,
        "192.168.1.1",
        8888,
        -1);
    PCMANAGER_ADDRemote(3,nei_node6);
// set upper admin
    PCMANAGER_ShowMap();
printf("\n===============================\n");
//    PCMANAGER_ReplaceID(3,10);
    
    PCMANAGER_RemoveNode(3);
    PCMANAGER_ShowMap();
//    PCMANAGER_UpperAdminLost();
//printf("=====lost \n");
//    PCMANAGER_ShowMap();
//    PCMANAGER_SETUpperAdmin(2);
}


//typedef struct testData{
//    char node_data;
//}TestData,*pTestData;
//
////*************************************************************
//pTestData createData(char c){
//    pTestData data = (pTestData)malloc(sizeof(TestData));
//    data->node_data = c;
//    return data;
//}
//
//pTestData copyNode(pTestData old){
//    return createData(old->node_data);
//}
//
//void freeNode(pTestData node){
//    printf("free node %c now\n",node->node_data);
//    node->node_data = '\0';
//    free(node);
//}
//
//void printNode(pTestData node){
//    printf("node->c = %c\n",node->node_data);
//    return;
//}
////*************************************************************
//int NodeFree(pNodeData node){
//    freeNode((pTestData)node);
//    return 1;
//}
//
//pNodeData NodeCopy(pNodeData fromnode){
//    pNodeData tonode = (pNodeData) copyNode((pTestData) fromnode);
//    return tonode;
//}
//
//int NodePrint(pNodeData node){
//    printNode((pTestData)node);
//    return 1;
//}
////*************************************************************
//
//int main(){
//    pTreeManager head = Tree_Init_Manager(
//        NULL,NodeFree,NodeCopy);
//    Tree_BuildTreeRoot(head , 0 , (pNodeData)createData('A'));
//    Tree_InsertNode(head, 0, 1, (pNodeData)createData('B'));
//    Tree_InsertNode(head, 0, 2, (pNodeData)createData('C'));
//    Tree_InsertNode(head, 0, 3, (pNodeData)createData('D'));
//    Tree_InsertNode(head, 2, 4, (pNodeData)createData('E'));
//    Tree_InsertNode(head, 2, 5, (pNodeData)createData('F'));
//    Tree_InsertNode(head, 3, 6, (pNodeData)createData('G'));
//    Tree_Traversal(head,NodePrint);
//    printf("******************************************\n");
//    Tree_RemoveNode(head,2);
//    Tree_Traversal(head,NodePrint);
//
//    printf("******************************************\n");
//    Tree_ReplaceNode(head,3,(pNodeData)createData('Z'));
//    Tree_Traversal(head,NodePrint);
//}
//
//
//************** LISTNODE TEST
//int main(){
//    pLISTHEAD head = ListNode_BuildListHead(
//        NULL,NodeFree,NodeCopy);
//    ListNode_InsertNode(head,0,(pNodeData)createData('A'));
//    ListNode_InsertNode(head,1,(pNodeData)createData('B'));
//    ListNode_InsertNode(head,2,(pNodeData)createData('C'));
//    ListNode_InsertNode(head,3,(pNodeData)createData('D'));
//    ListNode_InsertNode(head,4,(pNodeData)createData('E'));
//    ListNode_Traversal(head, NodePrint);
//    printf("********************************");
//    ListNode_DeleteNode(head,3);
//    ListNode_Traversal(head, NodePrint);
//    printf("********************************");
//    ListNode_ReplaceNodeData(head,4,(pNodeData)createData('F'));
//    ListNode_Traversal(head, NodePrint);
//}
//
//************  END LISTNODE TEST
