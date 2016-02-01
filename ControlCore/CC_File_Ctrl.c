/**
* File Name :ControlCore/CC_File_Ctrl.c
* Author: rootkiter
* E-mail: rootkiter@rootkiter.com
* Created Time : 二  1/26 23:12:32 2016
*/
#include "CC_File_Ctrl.h"
#include "Cmd_Ctrl.h"

typedef struct file_trans{
    int cmdtype;
    int datalen;
}FILE_CMD,*pFILE_CMD;

#define FILE_MSG_OPEN_OK        1   // 远程开文件成功
#define FILE_MSG_OPEN_ERROR     2   // 开文件失败
#define FILE_MSG_ADD_DATA       3   // 向文件追加数据
#define FILE_MSG_ADD_DATA_OK    4   // 向文件追加数据成功
#define FILE_MSG_ADD_DATA_ERROR 5   // 向文件追加数据失败
#define FILE_MSG_END_FILE       6   // 文件传输结束
#define FILE_MSG_EXCPTION       7   // 异常消息



int m_pconn_sendInt(pPCConn conn,int num){
    char intbuf[4];
    API_m_itochar(num,intbuf,4);
    if(PCCONN_SENDDATA_ERROR == PCCONN_SendData(conn,intbuf,4)){
        return 0;
    }
    return 1;
}

int m_pconn_recvInt(pPCConn conn){
    char intbuf[4];
    int res;
    if(PCCONN_RECVDATA_ERROR == PCCONN_RecvData(conn,intbuf,4)){
        return 0;
    }
    res = API_m_chartoi(intbuf,4);
    return res;
}

#define MAX_FILE_READ_LEN  1000
int CC_FILE_Upload(int targetid,
    char *from_lfile,
    char *to_rfile)
{
    char data[MAX_FILE_READ_LEN];
    if ( MAX_FILE_NAME_LEN < strlen(from_lfile) ||
        MAX_FILE_NAME_LEN < strlen(to_rfile)){
        return CC_FILE_UPLOAD_ERROR;
    }
    int rfile = API_File_Read_Open(from_lfile);
    if (FILE_OPEN_ERROR == rfile){
        return CC_FILE_UPLOAD_ERROR;
    }
    pPCConn conn = CMDCTRL_BuildTargetSock(targetid,
        AGENT_SERVER_COMMAND_UPFILE);
    //  send file name 
    PCCONN_SendData(conn,to_rfile,MAX_FILE_NAME_LEN);
    //  readOpenState
    int remoteOpenState = m_pconn_recvInt(conn);
    if(remoteOpenState == FILE_MSG_OPEN_ERROR){
        Printf_Error("Open %s File Error",to_rfile);
        return CC_FILE_UPLOAD_ERROR;
    }
    //  send file data
    int read_size = API_File_Read(rfile,data,MAX_FILE_READ_LEN);
    while(FILE_READ_END != read_size){
        m_pconn_sendInt(conn,FILE_MSG_ADD_DATA);
        m_pconn_sendInt(conn,read_size);
        PCCONN_SendData(conn,data,read_size);
        read_size = API_File_Read(rfile,data,MAX_FILE_READ_LEN);
    }
    //  send file end
    m_pconn_sendInt(conn,FILE_MSG_END_FILE);
    API_File_Close(rfile);
    return CC_FILE_UPLOAD_OK;
}

int CC_onFile_upload(pPCConn conn){
    char *data=(char *)malloc(sizeof(char)*MAX_FILE_READ_LEN);
    int filenum,recvcmd,datalen,recvsize;
    char *tofile=(char *)malloc(sizeof(char)*MAX_FILE_NAME_LEN+2);
    if(conn == NULL){
        return 0;
    }
    recvsize = 
    PCCONN_RecvData(conn,tofile,MAX_FILE_NAME_LEN);
    filenum = API_File_Write_Open(tofile);
    if(filenum == FILE_OPEN_ERROR){
        m_pconn_sendInt(conn,FILE_MSG_OPEN_ERROR);
        return 0;
    }
    m_pconn_sendInt(conn,FILE_MSG_OPEN_OK);
    recvcmd = m_pconn_recvInt(conn);
    while(recvcmd != FILE_MSG_END_FILE){
        datalen=m_pconn_recvInt(conn);
        PCCONN_RecvData(conn,data,datalen);
        API_File_Write(filenum,data,datalen);
        recvcmd = m_pconn_recvInt(conn);
    }
    Printf_OK("UPFile CMD exec Ok !");
    API_File_Close(filenum);
    return 1;
}

int CC_FILE_Downfile(int targetid,char *from_rfile,char *to_lfile){ 
    char data[MAX_FILE_READ_LEN];
    int recvcmd;
    if(MAX_FILE_NAME_LEN < strlen(from_rfile)
        || MAX_FILE_NAME_LEN < strlen(to_lfile)){
        return CC_FILE_DOWNFILE_ERROR;
    }
    int wfile = API_File_Write_Open(to_lfile);
    if( FILE_OPEN_ERROR == wfile){
        return CC_FILE_DOWNFILE_ERROR;
    }
    pPCConn conn = CMDCTRL_BuildTargetSock(targetid,
        AGENT_SERVER_COMMAND_DOWNFILE);
    // send file name
    PCCONN_SendData(conn,from_rfile,MAX_FILE_NAME_LEN);
    int remotestate=m_pconn_recvInt(conn);
    if ( FILE_MSG_OPEN_ERROR == remotestate){
        Printf_Error("Open %s File error",from_rfile);
        return CC_FILE_DOWNFILE_ERROR;
    }
    // recv file data
    recvcmd = m_pconn_recvInt(conn);
    while( FILE_MSG_ADD_DATA == recvcmd ){
        int datalen=m_pconn_recvInt(conn);
        PCCONN_RecvData(conn,data,datalen);
        API_File_Write(wfile,data,datalen);
        recvcmd = m_pconn_recvInt(conn);
    }
    API_File_Close(wfile);
    return CC_FILE_DOWNFILE_OK;
}


int CC_onFile_Downfile(pPCConn conn){
    char data[MAX_FILE_READ_LEN];
    char from_rfile[MAX_FILE_NAME_LEN];
    if( conn == NULL){
        return CC_ONFILE_DOWNFILE_ERROR;
    }
    PCCONN_RecvData(conn,from_rfile,MAX_FILE_NAME_LEN);
    int rfile = API_File_Read_Open(from_rfile);
    if ( FILE_OPEN_ERROR == rfile ){
        m_pconn_sendInt(conn,FILE_MSG_OPEN_ERROR);
        return CC_ONFILE_DOWNFILE_ERROR;
    }
    m_pconn_sendInt(conn,FILE_MSG_OPEN_OK);
    int read_size = API_File_Read(rfile,data,MAX_FILE_READ_LEN);
    while(FILE_READ_END != read_size){
        m_pconn_sendInt(conn,FILE_MSG_ADD_DATA);
        m_pconn_sendInt(conn,read_size);
        PCCONN_SendData(conn,data,read_size);
        read_size = API_File_Read(rfile,data,MAX_FILE_READ_LEN);
    }
    //  send file end
    m_pconn_sendInt(conn,FILE_MSG_END_FILE);
    API_File_Close(rfile);
    return CC_FILE_UPLOAD_OK;
}
