#include "generic.h"
#include "ControlCore/CC_AgentConn.h"
#include "AgentManager/PCNodeInfo.h"
#include "global_lib.h"

///////////////////////////////////////////////////////////
#define MAX_READLINE_LEN  300
int agentnow = 0;
char remoteip[MAX_IP_ADDR_LEN] = "127.0.0.1";
int  remoteport = 8888;
///////////////////////////////////////////////////////////
const char *optstring="c:p:hvaq";
struct option opts[]={
    {"tohost" , required_argument, NULL,'c'},
    {"toport" , required_argument, NULL,'p'},
    {"help"   , required_argument, NULL,'h'},
    {"version", required_argument, NULL,'v'},
    {"about"  , required_argument, NULL,'a'},
    {"qu"     , required_argument, NULL,'q'},
    {0,0,0,0}
};

int whileMain();
int startWith(char *str1,char *);
int readline(char *desbuf,char stopchar,int maxlen);
int help();
int arghelp();

int main(int argc,char *argv[]){
    char c;
    int index; 
    CC_Agent_Init(PCTYPE_GetMyType(),"This node is Admin",IAM_ADMIN_NODE);
    if(argc == 1){
        arghelp();
        return 0;
    } 
    while((c=getopt_long(argc,argv,optstring,opts,&index))!=-1){
        switch (c){
            case 'c':
                strncpy(remoteip,optarg,MAX_IP_ADDR_LEN);
                break;
            case 'p':
                remoteport = atoi(optarg);
                break;
            case 'h':
                arghelp();
                exit(1);
            case 'a':
                about_fun();
                break;
            case 'v':
                version_fun();
                break;
            case 'q':
                qu_fun();
                break;
            case '?':
                Printf_Error(" arg is unknown");
                arghelp();
                break;
            default:
                Printf_OK("c is %c",c);
                break;
        }
    }
    if ( CC_AGENT_CONNECT_ERROR == 
        CC_Agent_Connect(0,remoteip,remoteport)){
        return 1;
    }
    whileMain();
    return 0;
}


//char localip[]  = "127.0.0.1";
//int  localip    = 9999;
char msgbesend[300];
int whileMain(){
    char line[MAX_READLINE_LEN];
    char inputformat[MAX_READLINE_LEN];
    printf(">>>> ");
    readline(line,'\n',MAX_READLINE_LEN);
    while(!startWith(line,"quit")){
        if(startWith(line,"sendmsg")){
            Printf_OK("send msg here");
            sscanf(line,"sendmsg %s",msgbesend);
            CC_Agent_SendMsg(agentnow,msgbesend,strlen(msgbesend));
        }
        else if(startWith(line,"help")){
            help();
        }
        else if(startWith(line,"show")){
            CC_Agent_ShowMap();
        }
        else if(startWith(line,"goto")){
            sscanf(line,"goto %d",&agentnow);
            Printf_OK("Current be contral ID is %d",agentnow);
        }
        else if(startWith(line,"listen")){
            Printf_OK("target agent listen new port here");
            Printf_OK("%s",line);
            sscanf(line,"listen %d",&remoteport);
            CC_Agent_Listen(agentnow,remoteport,30);
        }
        else if(startWith(line,"connect")){
            Printf_OK("target agent connect new agent");
            sprintf(inputformat,"connect %s%ds %s","%",MAX_IP_ADDR_LEN,"%d");
            sscanf(line,inputformat,remoteip,&remoteport);
            Printf_OK("remote ip   :%s\n",remoteip);
            Printf_OK("remote port :%d\n",remoteport);
            CC_Agent_Connect(agentnow,remoteip,remoteport);
        }
        else if(startWith(line,"socks")){
            Printf_OK("Start socks server from target agent");
            int socksport;
            sscanf(line,"socks %d",&socksport);
            Printf_OK("Start Socks from %d port,Server Agent is %d",
                socksport,agentnow);
            CC_Agent_StartSocks(agentnow,socksport,10000);
        }
        else if(startWith(line,"lcxtran")){
            int lcxport;
            char rip[MAX_IP_ADDR_LEN];
            int rport;
            sprintf(inputformat,"lcxtran %s %s%ds %s","%d","%",MAX_IP_ADDR_LEN,"%d");
//            Printf_OK(inputformat,"HelloWorld");
            sscanf(line,inputformat,&lcxport,rip,&rport);
            Printf_OK("locolport   :%d\n",lcxport);
            Printf_OK("remote ip   :%s\n",rip);
            Printf_OK("remote port :%d\n",rport);
            CC_Agent_LcxTran(agentnow,lcxport,rip,rport,10000);
        }
        else if(startWith(line,"shell")){
            int shellport;
            sscanf(line,"shell %d",&shellport);
            Printf_OK("band %d's shell on %d locol port.",agentnow,shellport);
            
            CC_Agent_StartShell(agentnow,shellport,200,10000);
        }
        else{
            Printf_OK("unknow cmd");
            Printf_OK("%s",line);
        }
        printf(">>>> ");
        readline(line,'\n',MAX_READLINE_LEN);
    }
    return 1;
}

int help(){
    MyPrintf("**************************************************************************");
    MyPrintf("                          BASE COMMAND");
    MyPrintf(" ------------------------------------------------------------------------     ");
    MyPrintf(" %-35s %s","0. help","This help text.");
    MyPrintf(" %-35s %s","1. show","Display agent map.");
    MyPrintf("==========================================================================");
    MyPrintf("                          AGENT CONTROL");
    MyPrintf(" ------------------------------------------------------------------------     ");
    MyPrintf(" %-35s %s","2. goto     [id]",       "Select id as target agent.");
    MyPrintf(" %-35s %s","3. listen   [port]",     "Start server port on target agent.");
    MyPrintf(" %-35s %s","4. connect  [ip] [port]","Connect new agent from target agent.");
    MyPrintf("==========================================================================");
    MyPrintf(" START A SERVER ON TARGET AGENT,THEN BIND IT WITH LOCAL PORT");
    MyPrintf(" ------------------------------------------------------------------------     ");
    MyPrintf(" %-35s %s","5. socks    [lport]",    "Start a socks server.");
    MyPrintf(" %-35s %s","6. lcxtran  [lport] [rhost] [rport]","Build a tunnel with remote host.");
    MyPrintf(" %-35s %s","7. shell    [lport]","Start a shell server.");
//    MyPrintf(" %-35s %s","8. upfile   [from_file] [to_file]","Upload file from local host.");
//    MyPrintf(" %-35s %s","9. downfile [from_file] [to_file]","Download file from target agent.");
    MyPrintf("**************************************************************************");
    return 0;
}

int readline(char *desbuf,char stopchar,int maxlen){
    int i=0;
    int result = 1;
    char c;
    scanf("%c",&c);
    while(c!=stopchar){
        if(i>maxlen){
            result = -1;
            break;
        }
        desbuf[i] = c;
        scanf("%c",&c);
        i++;
    }
    fflush(stdin);
    fflush(stdout);
    desbuf[i] = '\0';
    if(result == 1){
        return i;
    }
    return -1;
}

int startWith(char *string,char *startString){
// startWith("ABCDEFGHIJKLMN","ABC")
//   return 1;
    int i =0;
    if(strlen(startString)>strlen(string)){ return 0; }
    for(i=0;i<strlen(startString);i++){
        if(string[i] != startString[i]){
            return 0;
        }
    }
    return 1;
}

int arghelp(){
    MyPrintf("");
    MyPrintf("VERSION : %s",CURRENT_VERSION);
    MyPrintf("Eg: ");
    MyPrintf("\t$ ./xxx -h");
    MyPrintf("\t$ ./xxx -c [rhost] -p [rport]");
    MyPrintf("---------");
    MyPrintf("options :");
    MyPrintf("%4s %-8s %s","-c","tohost","Remote host address.");
    MyPrintf("%4s %-8s %s","-p","toport","The port on remote host.");
    MyPrintf("%4s %-8s %s","-h","help","This help page.");
    MyPrintf("%4s %-8s %s","-v","version","Show the version.");
    MyPrintf("%4s %-8s %s","-a","about","Show the about text.");
    return 1;
}
