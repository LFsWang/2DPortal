#ifndef UI_H
#define UI_H

#include<Windows.h>
#include"library\console_draw3.h"
#include"library\font.h"
#include"library\image.h"
#include"library\kb_input2.h"

#include"define.h"


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

void getInputFileName(char *buf,size_t sz){
    //TODO remore VBS.
    remove("file.txt");
    system("echo X = InputBox(\"檔案名稱\",\"InputFile\") > openfile.vbs");
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
    getInputFileName(buf,80);
    
    if( !read_map(pmpf,buf) )
    {
        MSGBOX("無法開啟檔案");
        LOG("Open File:%s Error!",buf);
        return false;
    }
    return false;
}
#endif
