#include "myLib.h"
#include "levels.h"

#include "images/Level1.h"


int MAP_WIDTH = 32*8;
    


void loadLevel(int level) {    
    voff = 0;
    hoff = 0;

    showLevel(level);
}



void showLevel(int level) {
   int i;
    
   /*****************************************************************
    *
    *  Step 1 -- Tile Images
    *
    *  This data tells us what each different tile will look like
    *  It will be copied into a character block
    *
    ****************************************************************/
   
   /*****************************************************************
    *
    *  Step 2 -- Screen Image (map)
    *
    *	The numbers in this array tell us which tile we want in each
    *  tile location on the screen. We will copy this data into a
    *  screen block
    *
    ****************************************************************/
    
   /*****************************************************************
    *
    *  Step 3 -- Palette
    *
    ****************************************************************/
   
    /*****************************************************************
     *
     *  Step 4 -- Store Tile Images
     *
     ****************************************************************/
    int nTiles = 256;
    int total = nTiles*64/2;
            
    switch(level) {
        case 1:
            for(i=0; i < total; i++) {	// There are 10 tiles. Each tile is described by 64 chars
                                       // but we are storing them two to a short so we are
                                       // storing data in 64/2 shorts per tile
                //CHARBLOCKBASE[0].tileimg[i] = myTileImages[i*2] | (myTileImages[i*2+1]<<8);
                CHARBLOCKBASE[0].tileimg[i] = Level1Tiles[i*2] | (Level1Tiles[i*2+1]<<8);
            }
            break;
    }
     
    /*****************************************************************
     *
     *  Step 5 -- Store map
     *
     ****************************************************************/
    int totalMap; 
    switch(level) {
        case 1:
            totalMap = 32*32; //1024
            loadMapBlock(0, Level1Map);
            
            for(i=0; i < totalMap; i++) {
                //SCREENBLOCKBASE[31].tilemap[i] = Level1Map[i];
                //SCREENBLOCKBASE[30].tilemap[i] = myScreenMap[i];
                SCREENBLOCKBASE[28].tilemap[i] = myBackgroundMap[i];
            }
            break;
     }
     
    /*****************************************************************
     *
     *  Step 6 -- Store palette
     *
     ****************************************************************/
     switch(level) {
        case 1:
            //loadPalette(myPalette);
            loadPalette(Level1Pal);
            break;
     }
     
    /*****************************************************************
     *
     *  Step 7 -- Set image controls
     *
     ****************************************************************/
    //REG_DISPCTL = MODE0 | BG0_ENABLE | BG1_ENABLE | BG2_ENABLE;
    //SETMODE(MODE0 | BG0_ENABLE | BG1_ENABLE | BG2_ENABLE | SPRITE_ENABLE);
    SETMODE(MODE0 | BG0_ENABLE | /*BG1_ENABLE |*/ BG2_ENABLE | SPRITE_ENABLE);
    
    REG_BG0HOFS = 0;
    REG_BG0VOFS = 0;
    
    // MyGUI: Texts,... Comment out this line to turn off BG0
    //REG_BG0CNT = BG_SIZE0 | SBB(31) | COLOR256 | CBB(0);
    // Game
    REG_BG0CNT = BG_SIZE0 | SBB(30) | COLOR256 | CBB(0);
    // Background
    REG_BG2CNT = BG_SIZE0 | SBB(28) | COLOR256 | CBB(0);


    // DeuXieme part ;)   // 30
    //REG_BG1CNT = BG_SIZE0 | SBB(31) | COLOR256 | CBB(0);    
    
}




void loadMapBlock(int bb, unsigned short levelMap[]) {
    int loadRows = 32;
    int loadCols = 32;
    
    //bb = 1;
    
    int i = 0;
    int r, c;
    int cIni = bb*32; //32;
    int cLast = cIni+loadCols;
    //for(i = 0; i < totalMap; i++) {
    for (r = 0; r < loadRows; r++) {
        for (c = cIni; c < cLast; c++) {
            SCREENBLOCKBASE[30].tilemap[i] = levelMap[r*MAP_WIDTH+c];
            i++;
        }
    }
    //}
}



void loadMapBlockCol(int col, unsigned short levelMap[]) {
    int loadRows = 32;
    int loadCols = 32;
        
    int i = 0;
    int r, c;
    int cLast = col+loadCols;

    //for(i = 0; i < totalMap; i++) {
    for (r = 0; r < loadRows; r++) {
        for (c = col; c < cLast; c++) {
            SCREENBLOCKBASE[30].tilemap[i] = levelMap[r*MAP_WIDTH+c];
            i++;
        }
    }
    //}
}


// Find the Value of a tile inside a Map
int loadTile(int row, int col, unsigned short levelMap[]) {
    int theTile = levelMap[row*MAP_WIDTH + col];
    
    return theTile;
}



int levelWidth() {
    return MAP_WIDTH * 8;    
}