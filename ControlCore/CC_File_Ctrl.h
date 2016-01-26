#ifndef _CC_FILE_CTRL_H_
#define _CC_FILE_CTRL_H_

#include "../generic.h"
#include "../AgentManager/PCConn.h"

#define CC_FILE_UPLOAD_ERROR  -1
#define CC_FILE_UPLOAD_OK      1
int CC_FILE_Upload(int targetid,char *from_lfile,char *to_rfile);

#define CC_ONFILE_UPLOAD_ERROR 0
int CC_onFile_upload(pPCConn conn);

#define CC_FILE_DOWNFILE_ERROR -1
#define CC_FILE_DOWNFILE_OK     1
int CC_FILE_Downfile(int targetid,char *from_rfile,char *to_lfile);

int CC_onFile_Downfile(pPCConn conn);

#endif
