#include<Windows.h>
#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
#include<assert.h>
#include<io.h>
#include<Vfw.h>

#include"library\console_draw3.h"
#include"library\font.h"
#include"library\image.h"
#include"library\kb_input2.h"
#include"library\audio.h"

#include"define.h"
#include"log.h"
#include"map.h"
#include"ui.h"
#include"img.h"


int main()
{
    //
    int ui_select_mod;
    int lastselect=0;
    Mapfile mf;
    //intro
    log_intro();
    ui_intro();
    img_intro();
    //ui_showvideo();
    for(;;){
        ui_select_mod = ui_select(&lastselect);
        if( ui_select_mod == UI_EXIT ){
            break;
        }
        
        switch(ui_select_mod){
            case UI_MAP_EDITOR:
                if(ui_select_map(&mf)) 
                    ui_mapeditor(&mf);
                
                break;
            default:
                LOG("Unknow Ui Mode:%d",ui_select_mod);
                LOG("MsgBox:%d",MSGBOX("Unknow Ui Mode"));
                LOG("MsgBox:%d",GetLastError());
                
                break;
        }
        Sleep(100);
    }
    
	return 0;
}
