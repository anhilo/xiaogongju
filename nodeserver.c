#include "generic.h"
#include"AgentManager/AgentInteractive.h"
#include"AgentManager/AgentCTRL.h"

//int fun_newnode(int sock){
//    MyPrintf("new node here %d.",sock);
//    return 1;
//}
//
//int fun_newtunnel(int sock){
//    MyPrintf("new tunnel %d.",sock);
//    return 1;
//}
//
//int fun_tunnelans(int sock){
//    MyPrintf("tunnel answer %d.",sock);
//    return 1;
//}
//
//int fun_broadcast(int sock){
//    MyPrintf("broadcast %d.",sock);
//    return 1;
//}

int main(){
    AGENTCTRL_Init_Node(
        "The Server Node.",
        1);
    AGENTCTRL_StartServer(8888,200);                                  
    while(1){
        MIC_SLEEP(10000);
    }
    return 0;
}
