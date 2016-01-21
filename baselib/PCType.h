#ifndef _PCTYPE_H_
#define _PCTYPE_H_

#include "../generic.h"

#define OSTYPE_LINUX_OS       1
#define OSTYPE_ARMLINUX_OS    2
#define OSTYPE_WINDOW_OS      3
#define OSTYPE_MACOSX_OS      4
#define OSTYPE_UNKNOW_OS      5

int PCTYPE_GetMyType();

char PCTYPE_GetNodeFirstByte(int nodetype);

char *PCTYPE_BuildPCTypeString(int nodetype);

#endif
