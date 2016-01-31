#include "myLib.h"

#ifndef _LEVELS_H_
#define	_LEVELS_H_

//#define MAP_WIDTH = 32*4;
#define LEVEL_WIDTH 256*8;    // (MAP_WIDTH*8);  //(240+16)*4   /* MAXWIDTH + YELLOW */


//extern const unsigned char fontdata_6x8[12288];
extern const unsigned char myTileImages[];
extern const unsigned short myScreenMap[];
extern const unsigned short myBackgroundMap[];
extern const u16 myPalette[];

int voff;	// These two variables store our values for vertical
int hoff;	// and horizontal offset. During vblank we will
                // store these values into the appropriate hardware
                // register
int small_hoff;     
int small_voff;


void loadLevel(int level);
void showLevel(int level);

void loadMapBlock(int bb, unsigned short levelMap[]);


#endif	/* _LEVELS_H_ */

