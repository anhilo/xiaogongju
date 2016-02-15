/**
* File Name :global_lib.c
* Author: rootkiter
* E-mail: rootkiter@rootkiter.com
* Created Time : 一  1/25 00:47:16 2016
*/
#include "global_lib.h"

int qu_fun(){
    MyPrintf("\n");
    MyPrintf("          .\"'\".");
    MyPrintf("      .-./ _=_ \\.-.");
    MyPrintf("     {  (,(oYo),) }}");
    MyPrintf("     {{ |   \"   |} }");
    MyPrintf("     { { \\(---)/  }}");
    MyPrintf("     {{  }'-=-'{ } }");
    MyPrintf("     { { }._:_.{  }}");
    MyPrintf("     {{  } -:- { } }");
    MyPrintf("     {_{ }'==='{  _}");
    MyPrintf("    ((((\\)     (/))))");
    return 1;
}

int about_fun(){
    MyPrintf(" %s is a Machine Control Tool.It has many advantages.",CURRENT_TOOLNAME);
    MyPrintf(" There is a level-1 tool before ,you can find it from ");
    MyPrintf(" http://www.rootkiter.com/EarthWorm/ .");
    MyPrintf(" On the basis of 'EarthWorm',I added a built-in shell,");
    MyPrintf(" then add so much commands there. You can find more ");
    MyPrintf(" discription by add '-h' and '-d' parameter.");
    MyPrintf(" ");
    MyPrintf(" Contributors");
    MyPrintf(" rootkiter : The creator");
    MyPrintf(" wooyaa    : Proviede some advice");
    MyPrintf(" name1?    : Modify the Readme file");
    MyPrintf(" name2?    : English translation");
    MyPrintf(" name3?    : Named for this tool");
    return 1;
}

int detailed_fun(){
    MyPrintf(" ");
    MyPrintf(" %s is a Machine Control Tool.It has many advantages.",CURRENT_TOOLNAME);
    MyPrintf(" ");
    MyPrintf(" 1. You can control multiple hosts at the same time");
    MyPrintf("   In \"admin_exe\" there is a built-in shell.");
    MyPrintf("   So that,you can do different operation at the same time.");
    MyPrintf(" ");
    MyPrintf(" 2. It support Multiple control command.");
    MyPrintf("    1. Lcx_Tran       2. SOCKSv5 Server");
    MyPrintf("    3. Shell-Server   4. Upload file ");
    MyPrintf("    5. Download file  ");
    MyPrintf(" ");
    MyPrintf(" 3. You can manage Common e-machine.");
    MyPrintf("   It support various OS or CPU.For example:"); 
    MyPrintf("        Linux  (x86/x64/Arm/Mipsel);");
    MyPrintf("        Windows(x86/x64);");
    MyPrintf("        MacOS  (x64);");
    MyPrintf("       More is coming...");
    MyPrintf(" ");
    MyPrintf(" 4. You can use it on the Intranet or Extranet.");
    MyPrintf("   Eg:");
    MyPrintf("   4-1: When target has public IP,manage it with direct mode.");
    MyPrintf("      a-step) Run agent on target host:");
    MyPrintf("        $ ./agent_exe -l 8888");
    MyPrintf("      b-step) Manage it with connect it");
    MyPrintf("        $ ./admin_exe -c [target-ip] -p 8888 .");
    MyPrintf("   4-2. When target in a remote Extranet.You can Manage it");
    MyPrintf("        through a third-HOST(With public IP).");
    MyPrintf("      a-step) Run agent on third-HOST:");
    MyPrintf("        $ ./agent_exe -l 8888");
    MyPrintf("      b-step) back-connect third-HOST from target with agent");
    MyPrintf("        $ ./agent_exe -c [third-HOST ip] -p 8888");
    MyPrintf("      c-step) Manage target through third-HOST");
    MyPrintf("        $ ./admin_exe -c [third-HOST ip] -p 8888");
    MyPrintf(" ");
    MyPrintf(" 5. You can manage remote hosts through a multi-level cascade.");
    MyPrintf("   In the build-in shell,there is a \"connect\" or \"listen\" ");
    MyPrintf("   command,you can use it recv another agent,then manage the  ");
    MyPrintf("   new agent together.");
    MyPrintf(" ");
    return 1;
}

int version_fun(){
    MyPrintf("\nVERSION : %s",CURRENT_VERSION);
    return 1;
}
