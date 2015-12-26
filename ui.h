#ifndef UI_H
#define UI_H

#include<Windows.h>
#include"library\console_draw3.h"
#include"library\font.h"
#include"library\image.h"
#include"library\kb_input2.h"

#include"define.h"
#include"img.h"

pFont LG_FONT_DATA;
#define WIN_W 120
#define WIN_H 39
void ui_intro(){
    system("mode 120,39");
    SetConsoleTitle(GAME_NAME);
    initializeKeyInput();
    LG_FONT_DATA = read_font("font.txt");
}

void ui_showvideo(){
    Sleep(1000);
    HWND hWnd = FindWindow(NULL,GAME_NAME);
    HWND hAVI = MCIWndCreate(hWnd,NULL,WS_CHILD|WS_VISIBLE|MCIWNDF_NOMENU|MCIWNDF_NOPLAYBAR,NULL);
	SetWindowPos(hAVI,HWND_TOP,155,50,0,0,SWP_SHOWWINDOW);
	MCIWndOpen(hAVI,".\\logo.wmv",NULL);
	MCIWndPlay(hAVI);
	Sleep(6000);
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
    return botton[select].id;
}
void GetInputVBS(const char* title,char *buf,size_t sz){
    char tmp[256];
    sprintf(tmp,"echo X = InputBox(\"%s\",\"InputFile\") > openfile.vbs",title);
    remove("file.txt");
    system(tmp);
    system("echo Set fs = CreateObject(\"Scripting.FileSystemObject\") >>openfile.vbs");
    system("echo Set a = fs.CreateTextFile(\"file.txt\",True,False) >>openfile.vbs");
    system("echo a.WriteLine(\"map\\\"+X) >>openfile.vbs");
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

bool ui_select_map(pMapfile pmpf){
    char buf[80];
    GetInputVBS("檔案名稱",buf,80);
    if( !read_map(pmpf,buf) )
    {
        MSGBOX("無法開啟檔案");
        LOG("Open File:%s Error!",buf);
        return false;
    }
    return true;
}

bool draw_map(pMapfile mf,int X,int Y)
{
    LOG("DrowMapFile");
    int WBlock = WIN_W / 3;
    int HBlock = WIN_H / 3 - 2;
    int i,j,n,m;
    pBlock tblock = mf->block;
    clearScreen();
    LOG("%d %d",HBlock,mf->H);
    LOG("%d %d",WBlock,mf->W);
    for(i=Y,n=0;i<MIN(HBlock,mf->H);++i,++n)
        for(j=X,m=0;j<MIN(WBlock,mf->W);++j,++m)
            putImageById(m,n,tblock[i*mf->W+j].imgid);
}

bool ui_mapeditor(pMapfile mf)
{
    LOG("Modify Map");
    int posX = 0,posY = 0;
    bool exit = false;
    char buf[256];
    int ta,tb;
    
    while( !exit ){
        draw_map(mf,0,0);
        //nowpos
        putASCII2(posX*6,posY*3,'@',12,12);
        putASCII2(posX*6+1,posY*3,'@',12,12);
        
        sprintf(buf,"now pos %2d:%2d [%2d:%2d]",posY,posX,mf->H,mf->W);
        putString(0,34,buf,0,15);
        drawCmdWindow();
        
        while(!waitForKeyDown(5));
        int vcode = getKeyEventVirtual();
        switch(vcode){
            case VK_UP:     posY = MAX(0,posY-1);break;
            case VK_DOWN:   posY = MIN(mf->H-1,posY+1);break;
            case VK_LEFT:   posX = MAX(0,posX-1);break;
            case VK_RIGHT:  posX = MIN(mf->W-1,posX+1);break;
            case VK_ESCAPE: exit = true; break;
            case VK_R: 
                GetInputVBS("重新設定地圖尺寸 H W",buf,256);
                sscanf(buf,"%d %d",&ta,&tb);
                if( 0<ta&&ta<500 && 0<tb&&tb<500 ){
                    LOG("Resize to %d %d",ta,tb);
                    resize_map(mf,ta,tb);
                }
                else{
                    MSGBOX("尺寸需小於500x500"); 
                }
                
                break;
        }
    }
    //exit
}

#endif
