#ifndef _CC_LCXCTRL_H_
#define _CC_LCXCTRL_H_

#include "../generic.h"

#include "../AgentManager/GlobalValue.h"

#define CC_LCX_TRAN_ERROR   -1
#define CC_LCX_TRAN_OK       1
int CC_Lcx_Tran(int targetid,int locolport,char *rip,int rport,int usec);

#define CC_ONLCXTRAN_ERROR   0
int CC_onLcxTran(pPCConn conn);

#endif
