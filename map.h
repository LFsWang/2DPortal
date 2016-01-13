#ifndef MAP_H
#define MAP_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAPFILEVER "MAPVER1.0"
#define MAPFILEVERLEN sizeof(MAPFILEVER)
/*a map file struct 1.0
    format ver
    imgnum
    img
    
    H W
    H*W blocks
    id => Strings
*/
#define BLOCK_W 6
#define BLOCK_H 3

#define ET_TYPE_EMPTY   0
#define ET_TYPE_LOADMAP 1

typedef struct _Event{
    int type;
    void *data;
    struct _Event *next;
}Event,*pEvent;

//bit map
// 00000000
#define BK_TYPE_EMPTY   0
#define BK_TYPE_WELL    1
#define BK_TYPE_ENTER   2
#define BK_TYPE_END     3
typedef struct{
    int imgid;
    int type;
    int eventid;
}Block,*pBlock;

typedef struct{
    int event_num;
    pEvent event;
    int W,H;
    pBlock block;
}Mapfile,*pMapfile;

typedef bool (*callback)(pEvent,void*);
typedef bool (*runcallback)(pEvent,pMapfile,int*,int*);


#define FUNC_NUM 10
callback EV_CB_LOAD[FUNC_NUM];
callback EV_CB_SAVE[FUNC_NUM];
runcallback EV_CB_RUN[FUNC_NUM];

bool runEvent(pEvent p,pMapfile mf,int *ri,int *rj)
{
    //MSGBOX("run event!");
    if( EV_CB_RUN[p->type] == NULL )
    {
        MSGBOX("NULL HOOK FUNCTION");
    }
    else
    {
        if(!EV_CB_RUN[p->type](p,mf,ri,rj))
            return false;
    }
    return true;
}
bool ev_cb_load_0_msgbox(pEvent ptr,void *s)
{
    //MSGBOX((char*)s);
    char *str = (char*)s;
    ptr->next = NULL;
    ptr->type = 0;
    if( strlen(str) < 3 )
    {
        ptr->data = malloc(sizeof(char));
        ((char*)ptr->data)[0] = '\0';
    }
    else
    {
        str+=2;
        ptr->data = malloc(sizeof(char)*strlen(str)+1);
        memcpy(ptr->data,str,strlen(str)+1);
    }
    return true;
}

bool ev_cb_save_0_msgbox(pEvent ptr,void *s)
{
    FILE *f = (FILE*)s;
    fprintf(f,"0 %s",ptr->data);
    return true;
}
bool ev_cb_run_0_msgbox(pEvent ptr,pMapfile mf,int *ri,int *rj)
{
    MSGBOX((char*)ptr->data);
    return true;
}

bool ev_cb_load_1_loadmap(pEvent ptr,void *s)
{
    //MSGBOX((char*)s);
    char *str = (char*)s;
    ptr->next = NULL;
    ptr->type = 1;
    if( strlen(str) < 3 )
    {
        ptr->data = malloc(sizeof(char));
        ((char*)ptr->data)[0] = '\0';
    }
    else
    {
        str+=2;
        ptr->data = malloc(sizeof(char)*strlen(str)+1);
        memcpy(ptr->data,str,strlen(str)+1);
    }
    return true;
}

bool ev_cb_save_1_loadmap(pEvent ptr,void *s)
{
    FILE *f = (FILE*)s;
    fprintf(f,"1 %s",ptr->data);
    return true;
}

/******************/
char *_STR(char * str){
    size_t len = strlen(str);
    if( len > 0 ){
        len--;
        if( str[len] == '\n' )
            str[len] = 0;
    }
    return str;
}

bool resize_map(pMapfile mf,int nH,int nW)
{
    pBlock newmap = (pBlock)malloc(sizeof(Block)*nW*nH);
    pBlock oldmap = mf->block;
    memset(newmap,0,sizeof(Block)*nW*nH);
    
    if(!newmap) return false;
    
    int i,j;
    for( i=0;i < min(nH,mf->H) ;++i)
        for( j=0;j < min(nW,mf->W) ;++j)
            newmap[i*nW+j]=oldmap[i*mf->W+j];
    mf->block=newmap;
    free(oldmap);
    mf->W=nW;
    mf->H=nH;
    return true;
}
bool read_map(pMapfile ptr,const char *filename){
    int i,j;
    LOG("[File]Open %s",filename);
    FILE *fp = fopen(filename,"r");
    
    if( fp == NULL ){
        LOG("無法開啟檔案");
        return false;
    }
    //check header
    char headerbuf[MAPFILEVERLEN+1];
    fgets(headerbuf,MAPFILEVERLEN+1,fp);
    if( strcmp(MAPFILEVER,_STR(headerbuf)) != 0 ){
        LOG("檔案格式錯誤或是過時的格式");
        return false;
    }
    
    //load event
    fscanf(fp,"%d",&ptr->event_num);
    char buf[1024];
    if( ptr->event_num != 0 )
    {
        fscanf(fp,"%*c");
        pEvent tevent,tmp;
        tevent = ptr->event = (pEvent)calloc(ptr->event_num,sizeof(Event));
        for(i=0 ; i< ptr->event_num ; ++i){
            tmp=tevent+i;
            for(;;){
                if( !fgets(buf,sizeof(buf),fp) )
                {
                    LOG("event load fail with EOF");
                    return false;
                }
                _STR(buf); 
                int eid,rd;
                rd = sscanf(buf,"%d",&eid);
                LOG("event load %d:%d %s",i,eid,buf);
                if( rd != 1 ){
                    return false;
                }
                if( eid == -1 ){
                    break;
                }
                tmp->next = (pEvent)malloc(sizeof(Event));
                if( 0<=eid && eid<FUNC_NUM && EV_CB_LOAD[eid] != NULL )
                    EV_CB_LOAD[eid](tmp->next,buf);
                else{
                    MSGBOX("Function Hook Missing!");
                    LOG("Function Hook Missing! EV_CB_LOAD %d",eid);
                    return false;
                }
                tmp=tmp->next;
            }
        }
    }
    else
        ptr->event = NULL;
    
    //load block
    fscanf(fp,"%d %d",&ptr->H,&ptr->W);
    pBlock tblock;
    tblock = ptr->block = (pBlock)malloc(sizeof(Block)*ptr->H*ptr->W);
    
    for(i=0;i<ptr->H;++i){
        for(j=0;j<ptr->W;++j){
            fscanf(fp,"%d %d %d ",&tblock->imgid,&tblock->type,&tblock->eventid);
            tblock++;
        }
    }
    fclose(fp);
    return true;
}

bool save_map(pMapfile ptr,const char *filename){
    int i,j;
    LOG("[File]Save %s",filename);
    FILE *fp = fopen(filename,"w");
    fprintf(fp,MAPFILEVER "\n");
    //todo
    fprintf(fp,"%d\n",ptr->event_num);
    for(i=0;i<ptr->event_num;++i)
    {
        pEvent pe = ptr->event[i].next;
        while( pe != NULL )
        {
            EV_CB_SAVE[pe->type](pe,fp);
            fprintf(fp,"\n");
            pe=pe->next;
        }
        fprintf(fp,"-1\n");
    }
    pBlock tblock = ptr->block;
    fprintf(fp,"%d %d\n",ptr->H,ptr->W);
    for(i=0;i<ptr->H;++i){
        for(j=0;j<ptr->W;++j){
            fprintf(fp,"%d %d %d ",tblock->imgid,tblock->type,tblock->eventid);
            tblock++;
        }
        fprintf(fp,"\n");
    }
    fclose(fp);
    return true;
}
bool ev_cb_run_1_loadmap(pEvent ptr,pMapfile mf,int *ri,int *rj)
{
    MSGBOX("A");
    return false;
}
void map_intro()
{
    EV_CB_LOAD[0] = ev_cb_load_0_msgbox;
    EV_CB_SAVE[0] = ev_cb_save_0_msgbox;
    EV_CB_RUN [0] = ev_cb_run_0_msgbox;
    EV_CB_LOAD[1] = ev_cb_load_1_loadmap;
    EV_CB_SAVE[1] = ev_cb_save_1_loadmap;
    EV_CB_RUN [1] = ev_cb_run_1_loadmap;
}
#endif
