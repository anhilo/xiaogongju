#include "generic.h"
#include "AgentManager/PCNodeInfo.h"
#include "AgentManager/AgentConversationCTRL.h"
#include "AgentManager/PCNodeManager.h"
#include "ControlCore/CC_AgentConn.h"

#define VERSION_STR   "Alpha 2.0"
/////////////////////////////////////////////////////
#define MODE_UNKNOWN   0
#define MODE_SERVER    1 << 1
#define MODE_CLIENT    1 << 2
#define NEEDHELP       1 << 3
#define ABOUT_NOW      1 << 4
#define VERSION        1 << 5
#define FUNNY_FLAG     1 << 4

#define NOVALUESET     0
#define IP_BESETED     1 << 1
#define PORT_BESETED   1 << 2
#define HAS_STATE(m,n) (m&n)

int nodestate  = MODE_UNKNOWN;
int valuestate = NOVALUESET;
#define MAX_IP_ADDR_LEN_1 200
char host[MAX_IP_ADDR_LEN_1] = "127.0.0.1";
int  port = 8888;
/////////////////////////////////////////////////////
const char *optstring="l:c:p:hvaq";
struct option opts[]={
    {"listen" , required_argument, NULL,'l'},
    {"tohost" , required_argument, NULL,'c'},
    {"toport" , required_argument, NULL,'p'},
    {"help"   , required_argument, NULL,'h'},
    {"version", required_argument, NULL,'v'},
    {"aboutme", required_argument, NULL,'a'},
    {"qu"     , required_argument, NULL,'q'},
    {0,0,0,0}
};

int help();
int qu_fun();
int realDo();
int about_fun();

int mainDo(){
    if(HAS_STATE(nodestate,ABOUT_NOW)){
        about_fun();
    }
    if(HAS_STATE(nodestate,VERSION)){
        MyPrintf("\nVERSION : %s \n",VERSION_STR);
    }
    if(HAS_STATE(nodestate,FUNNY_FLAG) ){
        qu_fun();
    }
    if( HAS_STATE(nodestate,NEEDHELP) ){
        help();
    }
    else{
        realDo();
    }
    return 0;
}


int main(int argc,char *argv[]){
    char c;
    int index;
    API_env_init();
    AGENT_Conversation_Init(10,"This Client Node",MYSELF_NODE);
//    AGENT_Conversation_Connect("127.0.0.1",8888);

    if(argc == 1){ 
        help(); 
        return 0;
    }
    while((c=getopt_long(argc,argv,optstring,opts,&index))!=-1)
    {
        switch(c) {
        case 'l'://
            port = atoi(optarg);
            nodestate = nodestate | MODE_SERVER;
            break;
        case 'c'://
            strncpy(host,optarg,MAX_IP_ADDR_LEN_1);
            nodestate = nodestate | MODE_CLIENT;
            break;
        case 'p'://
            port = atoi(optarg);
            nodestate = nodestate | MODE_CLIENT;
            break;
        case 'h':
            nodestate = nodestate | NEEDHELP;
            break;
        case 'a':
            nodestate = nodestate | ABOUT_NOW;
            break;
        case 'v':
            nodestate = nodestate | VERSION;
            break;
        case 'q':
            nodestate = nodestate | FUNNY_FLAG;
            break;
        case '?'://选项未定义
            nodestate = nodestate | NEEDHELP;
            break;
        default:
            printf("c is %d\n",c);
            break;
        }
    }
    mainDo();
    return 0;
}



int realDo(){
    Printf_OK(" ip is %s",host);
    Printf_OK(" port is %d",port);

    if(HAS_STATE(nodestate,MODE_SERVER)){
        Printf_OK("Server State ...");
        Printf_OK("Listening --> 0.0.0.0:%d",port);
        CC_Agent_Listen(PCMANAGER_Get_RootID(),port,20);
    }
    else if(HAS_STATE(nodestate,MODE_CLIENT)){
        Printf_OK("Client State");
        Printf_OK("Connect to --> %s:%d",host,port);
        CC_Agent_Connect(PCMANAGER_Get_RootID(),host,port);
    }
    else{
        help();
        return 1;
    }
    Printf_OK("******MY ID is %d",PCMANAGER_Get_RootID());
    MIC_SLEEP(10);
    while(1){
        MIC_SLEEP(10000);
    }
    return 1;
}

int qu_fun(){
    MyPrintf("This is qu function");
    return 1;
}

int about_fun(){
    MyPrintf("This is about page");
    return 1;
}

int help(){
    MyPrintf("This is help page");
    return 1;
}

