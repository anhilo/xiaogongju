#include "generic.h"
#include "AgentManager/PCNodeInfo.h"
#include "AgentManager/AgentConversationCTRL.h"
#include "AgentManager/PCNodeManager.h"

int main(){
    API_env_init();
    AGENT_Conversation_Init(10,"This Client Node",IAM_ADMIN_NODE);
    AGENT_Conversation_Connect("127.0.0.1",8888);


    Printf_OK("******Chile here id now is %d",PCMANAGER_Get_RootID());





MIC_SLEEP(15);
Printf_DEBUG("start send");
    int sock2222 = AGENT_ConversationProxy_Build_Tunnel(2);
    char helloWorld[]="Hello This is from sock";
    API_socket_send(sock2222,helloWorld,strlen(helloWorld));
Printf_DEBUG("send ok");
////Printf_OK("Start Send BroadCast Msg");
////char buf[] = "Hello World BroadCast";
////SendDown_BroadCast(buf,strlen(buf));
////MIC_SLEEP(4);
////char buf2[] = "Hello World BroadCast--->2";
////SendDown_BroadCast(buf2,strlen(buf2));
////Printf_OK("Start Send OK");
//Broadcast_ReplaceID(30,20);
//
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
