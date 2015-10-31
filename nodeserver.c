#include "generic.h"
#include"AgentManager/AgentInteractive.c"

int fun_newnode(int sock){
    MyPrintf("new node here %d.",sock);
    return 1;
}

int fun_newtunnel(int sock){
    MyPrintf("new tunnel %d.",sock);
    return 1;
}

int fun_tunnelans(int sock){
    MyPrintf("tunnel answer %d.",sock);
    return 1;
}

int fun_broadcast(int sock){
    MyPrintf("broadcast %d.",sock);
    return 1;
}

int main(){
    AGENTINT_Init(fun_newnode,fun_newtunnel,fun_tunnelans,fun_broadcast);
    AGENTINT_StartServer(8888,20);
    while(1){
        MIC_SLEEP(10000);
    }
    return 0;
}
