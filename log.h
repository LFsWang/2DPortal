#ifndef LOG_H
#define LOG_H

#include<time.h>
#include<stdio.h>
#include<Windows.h>
#include"define.h"

#define LOG_BUF_SIZE 256
#define LOG(format,...) do{\
    fprintf(stderr,"[%s][%s:%d]\t" format "\n",log_time(),__FILE__,__LINE__,##__VA_ARGS__);\
    fflush(stderr);\
    }while(false)

//Why GetStdHandle return ERROR 1400 ?
#define MSGBOX(X) MessageBox(FindWindow(NULL,GAME_NAME),X,NULL,MB_OK)
char LOG_TIME_BUF[LOG_BUF_SIZE];
const char* log_time(){
    time_t rawtime;
    struct tm * timeinfo;
    time (&rawtime);
    timeinfo = localtime(&rawtime);
    strftime (LOG_TIME_BUF,LOG_BUF_SIZE,"%H:%M",timeinfo);
    return LOG_TIME_BUF;
}


void log_intro(){
    freopen("runtime.log","w",stderr);
    LOG("[LOG]log start");
}
#endif
