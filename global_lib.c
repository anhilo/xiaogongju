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
    MyPrintf(" %s is a Machine Control Tool.",CURRENT_TOOLNAME);

    MyPrintf(" 1. You can manage Common e-machine.");
    MyPrintf("   It support various OS or CPU:"); 
    MyPrintf("     Eg:");
    MyPrintf("        Linux  (x86/x64/Arm/Mipsel);");
    MyPrintf("        Windows(x86/x64);");
    MyPrintf("        MacOS  (x64);");
    MyPrintf("       More is coming...\n");

    MyPrintf(" 2. You can use it on the Intranet or Extranet.");
    MyPrintf("   2-1: When target has public IP,manage it with direct mode.");
    MyPrintf("    Eg:"
    Myprintf("      a-step) Run agent on target host:");
    Myprintf("        $ ./agent_exe -l 8888");
    Myprintf("      b-step) Manage it with connect it");
    Myprintf("        $ ./admin_exe -c [target-ip] -p 8888 .");
    MyPrintf("   2-2. When target in a remote Extranet.Manage it ");
    MyPrintf("        through third-HOST(With public IP).");
    
    MyPrintf("This is about page");
    return 1;
}

int version_fun(){
    MyPrintf("\nVERSION : %s",CURRENT_VERSION);
    return 1;
}
