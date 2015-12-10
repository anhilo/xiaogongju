#include "generic.h"
#include "AgentManager/PCNodeInfo.h"
#include "AgentManager/AgentConversationCTRL.h"
#include "AgentManager/PCNodeManager.h"

int sendMsgTo(int targetid){
    char buffer[200];
    sprintf(buffer,"Hello Client ->%d<-, This Msg send by Admin.",targetid);
    buffer[strlen(buffer)+1] = "\0";
Printf_DEBUG("start send %d",strlen(buffer));
    
    int sock = AGENT_Conversation_Build_SockTunnel(targetid);
    API_socket_send(sock,buffer,strlen(buffer)+1);
Printf_DEBUG("send ok");
    return 1;
}

int main(){
    API_env_init();
    AGENT_Conversation_Init(10,"This Admin Node",IAM_ADMIN_NODE);
    AGENT_Conversation_Connect("127.0.0.1",8888);

    Printf_OK("******my id now is %d",PCMANAGER_Get_RootID());

MIC_SLEEP(8);
    sendMsgTo(3);
MIC_SLEEP(8);
    sendMsgTo(2);

    while(1){
        MIC_SLEEP(10000);
    }
    return 0;
}

//int ConnectNode(){
//    char ip[MAX_IP_ADDR_LEN];
//    int  port;
//    MyPrintf("Please input the target url");
//    scanf("%s",ip);
//    
//    MyPrintf("Please input the target port");
//    scanf("%d",&port);
//}
