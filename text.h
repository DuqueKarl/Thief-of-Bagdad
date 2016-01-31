#ifndef _TEXT_H_
#define _TEXT_H_

extern const unsigned char fontdata_16x16[65536];

extern const int CHAR_WIDTH[128] = {
   /*0-15*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
  /*16-31*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
  /*32-47*/ 9, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 7, 0,  
  /*48-64*/10, 7, 7, 7, 7, 7, 7, 7,10,10, 0, 0, 0, 0, 0, 0,  
  /*64-79*/ 0,12,11,11,11,11,11,12,12, 7,11,11,10,14,11,11,  
  /*80-95*/10,11,12,12,11,10,12,13,11,11,11, 0, 0, 0, 0, 0,  
  /*96-12*/ 5, 9, 9, 9, 9, 9, 9, 9, 9, 8, 7, 9, 9,15,10, 9,  
  /*12-27*/ 9, 9, 9, 9, 9, 9, 9,12, 9,14, 9, 0, 0, 0, 0, 0
};



void drawChar(int row, int col, char ch, unsigned short color);
void drawString(int row, int col, char *str, unsigned short color);


#endif




