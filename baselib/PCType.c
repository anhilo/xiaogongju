/**
* File Name :baselib/PCType.c
* Author: rootkiter
* E-mail: rootkiter@rootkiter.com
* Created Time : 四  1/21 23:18:58 2016
*/
#include "PCType.h"

int PCTYPE_GetMyType(){
#ifdef WIN32
    return OSTYPE_WINDOW_OS;
#elif __APPLE__
    return OSTYPE_MACOSX_OS;
#elif __linux__
    #ifdef _ARM_
        return OSTYPE_ARMLINUX_OS;
    #else
        return OSTYPE_LINUX_OS;
    #endif
#else 
    return OSTYPE_UNKNOW_OS;
#endif
}


char PCTYPE_GetNodeFirstByte(int nodetype){
    switch(nodetype){
    case OSTYPE_LINUX_OS:
        return 'L';
    case OSTYPE_MACOSX_OS:
        return 'M';
    case OSTYPE_WINDOW_OS:
        return 'W';
    case OSTYPE_ARMLINUX_OS:
        return 'A';
    default:
        return 'U';
    }
}

#define MAX_OSTYPE_LEN  20
char *PCTYPE_BuildPCTypeString(int nodetype){
    char *res = (char *)malloc(sizeof(char)*MAX_OSTYPE_LEN);
    switch(nodetype){
    case OSTYPE_LINUX_OS:
        strncpy(res,"Linux",MAX_OSTYPE_LEN);
        break;
    case OSTYPE_MACOSX_OS:
        strncpy(res,"MacOSX",MAX_OSTYPE_LEN);
        break;
    case OSTYPE_WINDOW_OS:
        strncpy(res,"Windows",MAX_OSTYPE_LEN);
        break;
    case OSTYPE_ARMLINUX_OS:
        strncpy(res,"Arm-Linux",MAX_OSTYPE_LEN);
        break;
    default:
        strncpy(res,"Unknow-OS",MAX_OSTYPE_LEN);
        break;
    }
    return res;
}
