#ifndef IMG_H
#define IMG_H
#include<stdio.h>
#include<windows.h>
#include"library\image.h"
#define IMG_MAX 20

Image *img_list[IMG_MAX*20];
int __img_num=0;
void img_intro(){
    static char dir[266],dir2[266];
	__img_num = 0;
	for(;;){
	    sprintf(dir ,"img\\%d.pixel",__img_num);
	    sprintf(dir2,"img\\%d.color",__img_num);
	    if(_access(dir,F_OK)==0 && _access(dir2,F_OK)==0){
	        LOG("Load %d.img",__img_num);
	        img_list[__img_num]=read_image(dir,dir2);
	    }
	    else{
	        break;
	    }
	    __img_num++;
	}
}
void putImageById(int x,int y,int id){
	show_image(img_list[id],x*6,y*3);
}
Image *getImageById(int id){
    if( 0<=id && id<__img_num )
        return img_list[id];
    return NULL;
}
#endif
