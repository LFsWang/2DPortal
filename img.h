#include<stdio.h>
#include<windows.h>
#include"library\image.h"
#define IMG_MAX 20
int img_num;
Image *img_list[IMG_MAX*20];
bool img_intro(){
	img_num=0;
    static char dir[65535],dir2[65535];
    WIN32_FIND_DATA FileData;
    HANDLE hList;
    hList=FindFirstFile("img\\*", &FileData);
    if( hList==INVALID_HANDLE_VALUE ){
    	printf("No files be found.%d\n\n",GetLastError());
    }
    int is=0;
    while(FindNextFile(hList, &FileData)){
        sprintf(dir,"%s\\%s","img",FileData.cFileName);
        if(!strcmp("..",FileData.cFileName))continue;
        if(!strcmp(".",FileData.cFileName))continue;
		sprintf(dir2,"%s\\%s","img",FileData.cFileName);
        if(!strcmp("..",FileData.cFileName))continue;
        if(!strcmp(".",FileData.cFileName))continue;
		//printf("%s\n", dir);
		img_list[img_num++]=read_image(dir2,dir);
    }
    return 1;
}
void put(int x,int y,int id){
	show_image(img_list[id],x,y);
}

