#include "generic.h"
#include "AgentManager/PCNodeInfo.h"
#include "AgentManager/AgentConversationCTRL.h"
#include "AgentManager/PCNodeManager.h"

int main(){
    API_env_init();
    AGENT_Conversation_Init(10,"This Client Node",MYSELF_NODE);
    AGENT_Conversation_Connect("127.0.0.1",8888);


Printf_OK("******Chile here id now is %d",PCMANAGER_Get_RootID());


while(1){
    MIC_SLEEP(10000);
}
    return 0;
}
