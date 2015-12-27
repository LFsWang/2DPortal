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
putASCII2()  �̫��ӰѼƤ��O�O�r���e���M�I�����C��
�i�H�ѦҤU�����C��ȹ�Ӫ�ӳ]�w�A�Q�n���C��

   0: ��     1: �t��   2: �t��   3: �t�C
   4: �t��   5: �t��   6: �t��   7: �L��
   8: �t��   9: �G��  10: �G��  11: �G�C
  12: �G��  13: �G��  14: �G��  15: ��
*/

#define ARRAY_NUM(X) (sizeof(X)/sizeof(X[0]))
#define MIN(x,y) ((x)<(y)?(x):(y))
#define MAX(x,y) ((x)>(y)?(x):(y))
#endif
