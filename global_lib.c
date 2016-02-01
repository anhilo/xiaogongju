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
    MyPrintf("This is about page");
    return 1;
}

int version_fun(){
    MyPrintf("\nVERSION : %s",CURRENT_VERSION);
    return 1;
}
