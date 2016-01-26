#include "File_Base_API.h"

//*************************************************************8
//   FILE POOL CON
F_map filePool[MAX_FILE_POOL_NUM];
int SUM_FILE_POOL_NUM = 0;

int get_file_pool_num(){
    int i;
    for(i = 0;i< MAX_FILE_POOL_NUM;i++){
        if(filePool[i].busy != FILE_BUSY_STATE){
            filePool[i].busy = FILE_BUSY_STATE;
            SUM_FILE_POOL_NUM += 1;
            return i;
        }
    }
    return GET_FILE_POOL_NUM_ERROR;
}

int set_file_pool_pf(int poolnum,char *fname,FILE *p){
    strncpy( filePool[poolnum].filename, fname, MAX_FILE_NAME_LEN );
    filePool[poolnum].pfile = p;
    return 1;
}

FILE *get_pf(int poolnum){
    return filePool[poolnum].pfile;
}

int free_file_pool_num(int i){
    filePool[i].busy = FILE_NOT_BUSY_STATE;
    SUM_FILE_POOL_NUM -= 1;
    fclose(filePool[i].pfile);
    return 1;
}

//*************************************************************8
int API_Init_File_ENV(){
    int i;
    for (i =0 ;i< MAX_FILE_POOL_NUM;i++){
        filePool[i].busy = FILE_NOT_BUSY_STATE;
        SUM_FILE_POOL_NUM = 0;
        filePool[i].pfile = NULL;
    }
    return 1;
}


int API_File_Read_Open(char *name){
    FILE *pf;
    int poolnum;
    if( strlen(name) > MAX_FILE_NAME_LEN){
        return FILE_OPEN_ERROR;
    }
    pf = fopen(name,FILE_OPEN_READ_MODE);
    if( NULL == pf ){
        return FILE_OPEN_ERROR;
    }
    poolnum = get_file_pool_num();
    if(poolnum == GET_FILE_POOL_NUM_ERROR){
        fclose(pf);
        return FILE_OPEN_ERROR;
    }
    set_file_pool_pf(poolnum,name,pf);
    return poolnum;
}
    
int API_File_Write_Open(char *name){
    FILE *pf;
    int poolnum;
    if( strlen(name) > MAX_FILE_NAME_LEN){
        return FILE_OPEN_ERROR;
    }
    pf = fopen(name,FILE_OPEN_WRITE_MODE);
    if( NULL == pf ){
        return FILE_OPEN_ERROR;
    }
    poolnum = get_file_pool_num();
    if(poolnum == GET_FILE_POOL_NUM_ERROR){
        fclose(pf);
        return FILE_OPEN_ERROR;
    }
    set_file_pool_pf(poolnum,name,pf);
    return poolnum;
}

int API_File_Close(int fnum){
    free_file_pool_num(fnum);
    return 1;
}

int API_File_Write(int fnum,char *buf,int write_size){
    FILE *p = get_pf(fnum);
    fwrite(buf,1,write_size,p);
    return FILE_WRITE_OK;
}

int API_File_Read (int fnum,char *buf,int read_size){
    int real_size;
    if(read_size > MAX_READ_LEN){
        return FILE_READ_OVERLEN;
    }
    FILE *p = get_pf(fnum);
    real_size = fread(buf,1,read_size,p);
    if(real_size != read_size){
        if(real_size == 0){
            return FILE_READ_END;
        }
        else if (feof(p)){
            return real_size;
        }
        else{
            return FILE_READ_ERROR;
        }
    }
    return real_size;
}
