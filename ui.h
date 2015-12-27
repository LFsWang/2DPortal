#ifndef UI_H
#define UI_H

#include<Windows.h>
#include"library\console_draw3.h"
#include"library\font.h"
#include"library\image.h"
#include"library\kb_input2.h"

#include"define.h"
#include"img.h"


//My Music Function
void StopMusic(){
    mciSendString("stop Background",NULL,0,NULL);
	mciSendString("close Background",NULL,0,NULL);
}
void PlayAndRepeatMusic(const char *url)
{
    char tmp[500]="open ";
    strcat(tmp,url);
	strcat(tmp," alias Background");
	StopMusic();
	mciSendString(tmp,NULL,0,NULL);
	mciSendString("play Background repeat",NULL,0,NULL);
}

pFont LG_FONT_DATA;
Image * LOGOIMG;
Audio *A_BGM;
#define WIN_W 120
#define WIN_H 39
void ui_intro(){
    system("mode 120,39");
    SetConsoleTitle(GAME_NAME);
    
    LG_FONT_DATA = read_font("font.txt");
    SetWindowPos(FindWindow(NULL,GAME_NAME),HWND_TOP,10,10,0,0,SWP_NOSIZE);
    //LOGOIMG = read_image("img\\src\\j.pixel","img\\src\\j.color");
    LOG("%d",GetLastError());
    initializeKeyInput();
}

void ui_showvideo(){
    Sleep(1000);
    HWND hWnd = FindWindow(NULL,GAME_NAME);
    HWND hAVI = MCIWndCreate(hWnd,NULL,WS_CHILD|WS_VISIBLE|MCIWNDF_NOMENU|MCIWNDF_NOPLAYBAR,NULL);
	SetWindowPos(hAVI,HWND_TOP,155,50,0,0,SWP_SHOWWINDOW);
	MCIWndOpen(hAVI,".\\logo.wmv",NULL);
	MCIWndPlay(hAVI);
	Sleep(13500);
	MCIWndStop(hAVI);
	MCIWndClose(hAVI);
	MCIWndDestroy(hAVI);
}

#define UI_NONE -1
#define UI_EXIT 0
#define UI_SIGNAL_PLAYER 1
#define UI_MAP_EDITOR 2
typedef struct {
    char name[80];
    int id;
} UI_S_BUTTON;


int ui_select(int *lastselect){
    PlayAndRepeatMusic("src\\rock-bgm.mp3");
    // Counter
    int i;
    
    bool entered = false;
    UI_S_BUTTON botton[]={
        {"單人遊戲",UI_SIGNAL_PLAYER},
        {"地圖編輯器",UI_MAP_EDITOR},
        {"",UI_NONE},
        {"離開遊戲",UI_EXIT},
    };
    int select = 0;
    if( lastselect!= NULL )
        select = *lastselect;
    
    const int button_num = ARRAY_NUM(botton);
    
    while(!entered){
        clearScreen();
        int LOGO_X = ( WIN_W - sizeof(GAME_NAME)*LG_WORD_W ) / 2;
        putStringLarge(LG_FONT_DATA,LOGO_X,5,GAME_NAME,COLOR_LIGHTYELLOW);
        int row_y = 5+LG_WORD_H+3;
        for(i=0;i<button_num;++i){
            int word_color = COLOR_WHITE;
            if( select == i ){
                word_color = COLOR_LIGHTRED;
            }
            putString(50,row_y,botton[i].name,word_color,COLOR_BLACK);
            row_y+=2;
        }
        drawCmdWindow();
        while(waitForKeyDown(5)){
            int vcode = getKeyEventVirtual();
            switch(vcode){
                case VK_UP:
                    do{
                        select=(select-1+button_num)%button_num;
                    }while(botton[select].id == UI_NONE);
                    break;
                case VK_DOWN:
                    do{
                        select= (select+1)%button_num;    
                    }while(botton[select].id == UI_NONE);
                    break;
                case VK_RETURN:
                    if( botton[select].id != UI_NONE )
                        entered = true;
                    break;
            }
            break;
        }
    }
    if( lastselect != NULL )
        *lastselect = select;
    StopMusic();
    return botton[select].id;
}
void GetInputVBS(const char* title,char *buf,size_t sz){
    char tmp[256];
    sprintf(tmp,"echo X = InputBox(\"%s\",\"InputFile\") > openfile.vbs",title);
    remove("file.txt");
    system(tmp);
    system("echo Set fs = CreateObject(\"Scripting.FileSystemObject\") >>openfile.vbs");
    system("echo Set a = fs.CreateTextFile(\"file.txt\",True,False) >>openfile.vbs");
    system("echo a.WriteLine(X) >>openfile.vbs");
    system("echo a.Close >>openfile.vbs");
    
    system("openfile.vbs");
    while(_access("file.txt",F_OK)!=0){
        Sleep(100);
    }
    Sleep(50);
    
    FILE * f = fopen("file.txt","r");
    fgets(buf,sz,f);
    _STR(buf);
    fclose(f);
    remove("openfile.vbs");
}
char LAST_FILE_NAME[256];
bool ui_select_map(pMapfile pmpf){
    char tmp[80],buf[80]="map\\";
    GetInputVBS("檔案名稱",tmp,80);
    strcat(buf,tmp);
    
    if( !read_map(pmpf,buf) )
    {
        MSGBOX("無法開啟檔案");
        LOG("Open File:%s Error!",buf);
        return false;
    }
    strcpy(LAST_FILE_NAME,buf);
    return true;
}

void draw_map(pMapfile mf,int X,int Y)
{
    int WBlock = WIN_W / 3;
    int HBlock = WIN_H / 3 - 2;
    int i,j,n,m;
    pBlock tblock = mf->block;
    clearScreen();
    for(i=Y,n=0;i<mf->H && n<HBlock;++i,++n)
        for(j=X,m=0;j<mf->W&&m<WBlock;++j,++m)
            putImageById(m,n,tblock[i*mf->W+j].imgid);
}
void draw_map_md(pMapfile mf,int X,int Y,int *px,int *py){
    //set mid = (10,5)
    int nX = max(0,X-10);
    int nY = max(0,Y-5);
    draw_map(mf,nX,nY);
    if(px!=NULL)    *px = X-nX;
    if(py!=NULL)    *py = Y-nY;
}

#define DMODE_NONE -1
bool ui_mapeditor(pMapfile mf)
{
    LOG("Modify Map");
    int posX = 0,posY = 0;
    bool exit = false;
    char buf[256];
    int ta,tb;
    int drawerid = DMODE_NONE;
    int type = DMODE_NONE;
    
    while( !exit ){
        //draw_map
        //draw_map(mf,0,0);
        draw_map_md(mf,posX,posY,&ta,&tb);
        //drawer
        if( drawerid!=DMODE_NONE)
            show_image(getImageById(drawerid),0,34);
        sprintf(buf,"筆刷: %d.img",drawerid);
        putString(9,34,buf,15,0);
        sprintf(buf,"屬性: %d",type);
        putString(9,35,buf,15,0);
        
        putString(0,37,"O:筆刷 P:屬性",15,0);
        //nowpos
        putASCII2(ta*6,tb*3,'@',12,12);
        putASCII2(ta*6+1,tb*3,'@',12,12);        
        sprintf(buf,"now pos %2d:%2d [%2d:%2d]",posY,posX,mf->H,mf->W);
        putString(25,34,buf,0,15);
        
        //this point info
        pBlock nPos= &(mf->block[posX+posY*mf->W]);
        show_image(getImageById(nPos->imgid),50,34);
        sprintf(buf,"IMG :%d",nPos->imgid);
        putString(58,34,buf,0,15);
        sprintf(buf,"屬性:%d",nPos->type);
        putString(58,35,buf,0,15);
        sprintf(buf,"事件:%d",nPos->eventid);
        putString(58,36,buf,0,15);
        drawCmdWindow();
        
       
        while(!waitForKeyDown(5));
        int vcode = getKeyEventVirtual();
        switch(vcode){
            case VK_UP:     posY = MAX(0,posY-1);break;
            case VK_DOWN:   posY = MIN(mf->H-1,posY+1);break;
            case VK_LEFT:   posX = MAX(0,posX-1);break;
            case VK_RIGHT:  posX = MIN(mf->W-1,posX+1);break;
            case VK_ESCAPE: 
                ta = MSGBOXNY("是否離開? 將會遺失未儲存的資料");
                if( ta == IDYES )
                    exit = true; 
                break;
            //Drawer
            case VK_O:
                GetInputVBS("筆刷ID (-1設定為空)",buf,256);
                sscanf(buf,"%d",&ta);
                if( getImageById(ta) == NULL && ta!=DMODE_NONE ){
                    MSGBOX("無此筆刷");
                }
                else {
                    drawerid = ta;
                    LOG("更換筆刷為%d.img",ta); 
                }
                break;
            case VK_P:
                GetInputVBS("屬性ID (-1設定為空)",buf,256);
                sscanf(buf,"%d",&ta);
                if( (ta<0||ta>2) && ta!=DMODE_NONE ){
                    MSGBOX("無此屬性");
                }
                else {
                    type = ta;
                    LOG("更換屬性為%d",ta); 
                }
                break;
            case VK_SPACE:
                if( drawerid != DMODE_NONE )
                    mf->block[posX+posY*mf->W].imgid=drawerid;
                if( type != DMODE_NONE )
                    mf->block[posX+posY*mf->W].type=type;
                break;
            case VK_S:
                if( save_map(mf,LAST_FILE_NAME) ){
                    MSGBOX("存檔成功\"); 
                } else {
                    MSGBOX("存檔失敗");
                }
                break;
            case VK_R: 
                GetInputVBS("重新設定地圖尺寸 H W",buf,256);
                sscanf(buf,"%d %d",&ta,&tb);
                MSGBOX(buf);
                if( 0<ta&&ta<=500 && 0<tb&&tb<=500 ){
                    LOG("Resize to %d %d",ta,tb);
                    resize_map(mf,ta,tb);
                    posY = min(posY,ta);
                    posX = min(posX,tb);
                }
                else{
                    MSGBOX("尺寸需小於500x500"); 
                }
                break;
        }
    }
    //exit
}

bool ui_mapplay(){
    LOG("Play Game");
    int i,j,k;
    bool bad_error = false;
    bool exit_flag = false;
    int lx,ly;
    int ta,tb;
    //To do
    //Load Old Data
    Mapfile mf;
    pBlock mfb;
    if( !ui_select_map(&mf) ){
    //if( !read_map(&mf,"map\\a.txt") ){
        LOG("Fail Load Map");
        MSGBOX("地圖載入失敗!"); 
        return false;
    }
    mfb = mf.block;
    
    //TO DO move to img.h
    Image *img_player;
    img_player = read_image("img\\people\\people.pixel","img\\people\\people.color");
    
    int uPosX = 0,tPosX;
    int uPosY = 0,tPosY;
    
    //First Find enterence 
    //if saved ,go to old pos
    //if muilt entery it should random
    for(i=0;i<mf.H;++i)
        for(j=0;j<mf.W;++j)
            if( mfb[i*mf.W+j].type == BK_TYPE_ENTER ) {
                uPosX=j;
                uPosY=i;
            }
    LOG("Enter at %d,%d",uPosX,uPosY);
    while( !bad_error ){
        clearScreen();
        draw_map_md(&mf,uPosX,uPosY,&lx,&ly);
        show_image(img_player,lx*6,ly*3);
        drawCmdWindow();
        
        while(!waitForKeyDown(5));
        int vcode = getKeyEventVirtual();
        
        tPosX=uPosX;
        tPosY=uPosY;
        switch(vcode){
            case VK_UP:     tPosY = MAX(0,uPosY-1);break;
            case VK_DOWN:   tPosY = MIN(mf.H-1,uPosY+1);break;
            case VK_LEFT:   tPosX = MAX(0,uPosX-1);break;
            case VK_RIGHT:  tPosX = MIN(mf.W-1,uPosX+1);break;
            case VK_ESCAPE: 
                ta = MSGBOXNY("是否離開? 將會遺失未儲存的資料");
                if( ta == IDYES )
                    exit_flag = true; 
                break;
        }
        if( tPosX!=uPosX || tPosY != uPosY ){
            //moved
            if( (mfb[ tPosY*mf.W+tPosX ].type & 1) == 0 ){ //Can pass
                uPosX = tPosX;
                uPosY = tPosY;
            }
        }
        if( exit_flag ) break;
    }
    
    if( bad_error ){
        MSGBOX("遊戲發生嚴重錯誤，所有未儲存的資料將會遺失");
        return false;
    }
    return true;
}

#endif
