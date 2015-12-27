#ifndef DEFINE_H
#define DEFINE_H

#define GAME_NAME "Tower of KeLa"

typedef Font * pFont; 
#define LG_WORD_W 8
#define LG_WORD_H 5
#define COLOR_BLACK 0
#define COLOR_LIGHTRED  12
//#define COLOR_LIGHTRED  13
#define COLOR_LIGHTYELLOW 14
#define COLOR_WHITE     15
/*
putASCII2()  最後兩個參數分別是字元前景和背景的顏色
可以參考下面的顏色值對照表來設定你想要的顏色

   0: 黑     1: 暗藍   2: 暗綠   3: 暗青
   4: 暗紅   5: 暗紫   6: 暗黃   7: 淺灰
   8: 暗灰   9: 亮藍  10: 亮綠  11: 亮青
  12: 亮紅  13: 亮紫  14: 亮黃  15: 白
*/

#define ARRAY_NUM(X) (sizeof(X)/sizeof(X[0]))
#define MIN(x,y) ((x)<(y)?(x):(y))
#define MAX(x,y) ((x)>(y)?(x):(y))
#endif
