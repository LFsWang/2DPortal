#ifndef MAP_H
#define MAP_H

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

typedef struct{
    int type;
}Event,*pEvent;
//bit map
// 00000000
#define BK_TYPE_EMPTY   0
#define BK_TYPE_WELL    1
#define BK_TYPE_ENTER   2
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
    char buf[MAPFILEVERLEN+1];
    fgets(buf,MAPFILEVERLEN+1,fp);
    if( strcmp(MAPFILEVER,_STR(buf)) != 0 ){
        LOG("檔案格式錯誤或是過時的格式");
        return false;
    }
    
    //load event
    fscanf(fp,"%d",&ptr->event_num);
    LOG("NOT Support event_num now");
    assert( ptr->event_num == 0 );
    
    if( ptr->event_num != 0 )
    {
        pEvent tevent;
        tevent = ptr->event = (pEvent)malloc(sizeof(Event)*ptr->event_num);
        for(i=0;i<ptr->H;++i){
            //TODO
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
    fprintf(fp,"0\n");
    
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
#endif
