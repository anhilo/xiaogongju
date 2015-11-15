#include "generic.h"
#include"AgentManager/AgentConversationCTRL.h"
//#include"AgentManager/AgentCTRL.h"

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
    AGENT_Conversation_Init(20,"This is Server Node",1);
    AGENT_Conversation_Listen(8888,20);
    while(1){
        MIC_SLEEP(10000);
    }
    return 0;
}
