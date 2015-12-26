#include<Windows.h>
#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>

#include"library\console_draw3.h"
#include"library\font.h"
#include"library\image.h"
#include"library\kb_input2.h"
#include"library\audio.h"

#include"define.h"
#include"log.h"
#include"ui.h"



int main()
{
    //
    int ui_select_mod;
    int lastselect=0;
    //intro
    log_intro();
    ui_intro();
    
    for(;;){
        ui_select_mod = ui_select(&lastselect);
        if( ui_select_mod == UI_EXIT ){
            break;
        }
        
        switch(ui_select_mod){
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
