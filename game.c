#include "game.h"
#include "levels.h"
#include "myLib.h"
#include "sound.h"
#include "images/Level1.h"
#include "images/SpritesBT.h"

#include "sound/powerupsfx.h"
#include "sound/Snd_Scorpio.h"
#include "sound/Snd_Sword.h"
#include "sound/Snd_Knife.h"


#define ROWMASK 0xFF // to keep sprites on screen (prevents odd loop around glitches)
#define COLMASK 0x1FF
OBJ_ATTR shadowOAM[128];

#define FACING_RIGHT 0
#define FACING_LEFT 1
   
int timer = 0; // how many update frames per animationCounter
int animationCounter = 0; // used to determine animation frame 


int pTRow;
int pTCol;

int myCoins;

int lastBlockLoaded = 0;        // The last block we loaded

// CHECKPOINTS   MIN.DIST: 260
int checkPoints[] = { 530, 1100, 2000};
int totalCP = 3;



void initSprites() {
    loadSprites();

    initPlayer();
   
    // Init the enemies
    totalEnemies = 0;
    initEnemy(130, 180, 290, SCORPIO);
    initEnemy(48,   15,  85, SCORPIO);
    initEnemy(130,  85, 150, SCORPIO);
    
    initEnemy(48, 140, 220, SOLDIER1);
    initEnemy(130, 350, 390, SOLDIER1);

    // SecondPart
    initEnemy(130, 710, 785, SOLDIER1);
    initEnemy(48, 1284, 1360, SOLDIER1);

    initEnemy(48, 470, 475, THROWER1);
    initEnemy(130, 1005, 1010, THROWER1);
    
    //Second part
    initEnemy(130, 580, 660, SCORPIO);
    initEnemy(48, 902, 984, SCORPIO);
    initEnemy(130, 1250, 1360, SCORPIO);
    
    // CRAZY THING!!!!
    initEnemy(130, 1910, 1920, THROWER1);
    initEnemy(130, 1810, 1985, SOLDIER1);
    initEnemy(130, 1884, 1960, SOLDIER1);
    initEnemy(130, 1484, 1560, SOLDIER1);
    initEnemy(48, 1550, 1630, SCORPIO);
    
    // Init the carpets
    totalCarpets = 0;
    //initCarpet(48, 140, 350, HORIZONTAL);
    initCarpet(48, 1790, 1875, HORIZONTAL);
    //initCarpet(48, 1790+256, 1875+256, HORIZONTAL);
    
    
    // Init your coins
    totalCoins = 0;
    
    // Hide everything
    int i;
    for(i = 0; i < 128; i++) {
        OAM[i].attr0 = ATTR0_HIDE; 
        shadowOAM[i].attr0 = ATTR0_HIDE;
    }    
}



void initPlayer() {
    timerWalk = 0;
    animationWalk = 0;
    isJumping = false;
    jumpInc = 0;
    canJump = true;
    MAX_JUMP = 15.0f; //12.0f;
    collisions = 0;

    frontflip = 0;
    animationFlip = 0;

    playerX = 25; 
    playerY = 130; //130;
    
    // Test
    //playerX += 1500;
    
    // Load a CheckPoint
    if (checkPoint > -1) {
        playerX = checkPoints[checkPoint] + 25;
    }
    
    player.hflip = 0;
    player.alive = LIVE;
    lastFacing = FACING_RIGHT;
}



void loadSprites() {
    //DMANow(3, (unsigned int*)Pikachu7Tiles, &CHARBLOCKBASE[4], Pikachu7TilesLen/2);
    //DMANow(3, (unsigned int*)Pikachu7Pal, SPRITE_PALETTE, 256);
    
    //DMANow(3, (unsigned int*)ScorpioTiles, &CHARBLOCKBASE[4], ScorpioTilesLen/2);
    //DMANow(3, (unsigned int*)ScorpioPal, SPRITE_PALETTE, 256);

    //DMANow(3, (unsigned int*)textSheetTiles, &CHARBLOCKBASE[4]+textSheet[1].startPos, textSheetTilesLen/2);
    //DMANow(3, (unsigned int*)textSheetPal, SPRITE_PALETTE, 256);

    DMANow(3, (unsigned int*)SpritesBTTiles, &CHARBLOCKBASE[4], SpritesBTTilesLen/2);
    DMANow(3, (unsigned int*)SpritesBTPal, SPRITE_PALETTE, SpritesBTPalLen/2);
    
}


void loopGame(int lifes, int co) {
    int newBlock;
    
    myCoins = co;
    
    moveSprites();

    if (player.alive == LIVE) {
        movePlayer();
        checkCollisions();
    } else if (player.alive == DYING) {
        dyingPlayer();
    }
    
    showCP();
    showCoins();
    showCarpets();
    putLifes(lifes);
    putMyCoins(myCoins);
    
    DMANow(3, shadowOAM, OAM, 128*4);


    
    small_hoff = hoff%8;        
    newBlock = hoff / 8;
    if (newBlock != lastBlockLoaded) {
        lastBlockLoaded = newBlock;
        loadMapBlockCol(hoff/8, Level1Map);
    }
    
    REG_BG0HOFS = small_hoff;
    //REG_BG0HOFS = small_hoff;
    //REG_BG0HOFS = hoff;
    REG_BG0VOFS = voff;  
    REG_BG2HOFS = hoff >> 4;
    REG_BG2VOFS = voff >> 4;    
}



void moveSprites() {
    timer++; 
                
    if(timer == 9){ // next animation every 10 update frames 
        timer = 0; 
        animationCounter++; 
    }

    
    int i;
    for (i = 1; i <= totalEnemies; i++) {
        if (enemies[i].alive == DEAD) {
            continue;
        }
        
        if (enemies[i].alive == LIVE || enemies[i].type == KNIFE) {
            moveEnemy(i);
        } else {
            dyingEnemy(i);
        }
    }
    
}    


void movePlayer() {
    
    // PLAYER
    int moving = 0;
    int playerFacing = lastFacing;  //PIKAFACING_FRONT;

    // Running!!!-------
    if (KEY_DOWN_NOW(BUTTON_RIGHT)) {
        playerFacing = FACING_RIGHT;
        timerWalk++;
        if (timerWalk == 5) {
            timerWalk = 0;
            animationWalk++;
        }
        moving = true;
    }
    if (KEY_DOWN_NOW(BUTTON_LEFT)) {
        playerFacing = FACING_LEFT;
        timerWalk++;
        if (timerWalk == 5) {
            timerWalk = 0;
            animationWalk++;
        }
        moving = true;
    }
    // End of Running------
    
    // Stance Animation (Not moving)
    if (moving == 0) {
        timerWalk = 0;
        animationWalk = 0;
    }
    
    // Make the camera follow the player (SCROLL)
    if (playerX > levelWidth() - 120) {
        //hoff = levelWidth() - 256;
    } else if (playerX > 120) {
        hoff = playerX - 120;
    } else {
        hoff = 0;
        small_hoff = 0;
    }
    
    /*
    // Now the vertical!
    int voff1 = playerY - 80;
    int MAX_VOFF = 0;
    int MIN_VOFF = -80;
    // Smooth it
    if (voff1 >= MIN_VOFF && voff1 <= MAX_VOFF) {
        if (voff < voff1) { voff+=2; }
        else if (voff > voff1) { voff-=2; }
    }
    */
    
    
    // Save the CHECK POINTS!!!
    if (playerX > checkPoints[checkPoint+1]) {
        checkPoint++;
    }
    
    
    // Jumping------------
    //continue jumping if already in the air
    if (isJumping) {
        canJump = 0;
        timerJump++;
        if (timerJump >= 4) {
            timerJump = 0;
            
            // -24, -19, -15...
            if (jumpInc < 0) {
                jumpInc *= 0.82; //(1.0 - (MAX_JUMP / 125.0f));
                if (jumpInc > -4) { //(-MAX_JUMP / 5.0f)) {
                    //jumpInc *= -1.0;
                    jumpInc = 5; 
                }
            }
            if (jumpInc > 0 && jumpInc <= MAX_JUMP) {
                jumpInc *= 1.1; //1.0 + (MAX_JUMP / 175.0f);
            }
            if (jumpInc > 8) jumpInc = 8;
            playerY += jumpInc;            
            animationWalk = 0;
        }
    } 
    // End of Jumping----------
    
    int finishedJump = false;
    

    /***************/
    // Check if the player is colliding with a Flying Carpet
    int flying = false;
    int i;
    for (i = 0; i < totalCarpets; i++) {
        if (carpets[i].col >= playerX - 60 && carpets[i].col <= playerX - 8) {
            //if (carpets[i].row >= playerY - 24 && carpets[i].row - 16 >= playerY) {
            if (playerY - 24 <= carpets[i].row && playerY >= carpets[i].row - 16) {
                playerY = carpets[i].row - 16;
                if (carpets[i].timerMov < 1000) {
                    playerX--;
                } else {
                    playerX++;
                }
                flying = true;
                finishedJump = true;
            }
        }
    }
    /***************/
    
    
    // Check player's position (get the TILE where he is at)
    pTRow = (playerY+16) >> 3;
    pTCol = (playerX+8) >> 3;
        
    // New Tracer
    //int theTileUp = loadTile(pTRow-1, pTCol, Level1Map);
    int theTileM = loadTile(pTRow, pTCol, Level1Map);
    //int theTileDn = loadTile(pTRow+1, pTCol, Level1Map);
    if (false) {
       // SCREENBLOCKBASE[30].tilemap[11] = theTileUp;
        SCREENBLOCKBASE[30].tilemap[43] = theTileM;
        //SCREENBLOCKBASE[30].tilemap[75] = theTileDn;
    }

    
    // If player is moving down (jumping in the air)
    // This is also GRAVITY
    if (jumpInc >= 1 && !flying) {        
        
        // If he touches the Floor, land him, stop falling
        if (theTileM == FL2) {
        //if (theTile == FL2) {
            finishedJump = true;
            playerY = 130;

        // On the roof!!!!
        } else if ( (theTileM >= 1 && theTileM <= 7 )) {
            finishedJump = true;
            playerY = pTRow*8 - 16;
        
        // Landing on a Market...
        } else if ((theTileM >= K2n && theTileM <= K5n)
            || (theTileM >= K1b && theTileM <= K6b)
            || (theTileM >= K1a && theTileM <= K6a)){
            finishedJump = true;
            playerY = pTRow*8 - 16;     
            
        // He continues in the air falling...
        } else {
            //Falling
            canJump = false;
            isJumping = true;
            frontflip = 2;      // Never do a frontflip falling
        }

        // Y=130 is always the Floor
        if (playerY >= 130) {
            finishedJump = true;
            playerY = 130;
        }        
    }
    
    if (finishedJump) {
        jumpInc = 5;
        isJumping = false;
        canJump = true;
        frontflip = 0;
        animationFlip = 0;
    }

    
    
    //(animationWalk%4)*2
    int facing = 0;
    if (playerFacing == FACING_LEFT) facing = ATTR1_HFLIP;
    // Hide the other parts...
    moveHero(TI2, 160, 240, 0, 0, 0);
       
    // Draw the good frame!
    int offR = 0;
    int offC = 0; 
    
    // Jumping?
    if (isJumping) {
        if ((!moving && frontflip == 0) || (frontflip == 2)) {
            offR = 0;
            offC = 2;
        } else {
            frontflip = 1;
            if (timerJump == 0) {
                animationFlip++;
            }
            offR = 0;
            offC = 2 + (animationFlip%6)*2;
            // Only one frontflip animation...
            if (animationFlip == 6) { frontflip = 2; }
        }
    
        // Update player position!
        moveHero(0, playerY - 24, playerX, offR, offC, ATTR1_SIZE32 | facing);
    }
    // Running
    else if (moving) {
        int anim = animationWalk%12; 
        offR = 4;
        int offC2 = -1;
        switch(anim) {
            case 0: case 1: case 2: case 3:
                offC = (anim)*2;
                break;
            case 4:
                offC = (anim*2);
                offC2 = offC+1;
                break;
            case 5:
                offC = 11;
                offC2 = 12;
                break;
            case 6: case 7: case 8:
                offC = 14+(anim-6)*2;
                break;
            case 9: case 10: case 11:
                offC = 20+(anim-9)*3;
                offC2 = offC+1;
                break;
        }
        // Update player position!
        if (offC2 == -1) {
            moveHero(0, playerY - 16, playerX, offR, offC, ATTR1_SIZE32 | facing);
        } else {
            if (facing == 0) {
                moveHero(0, playerY - 16, playerX -4, offR, offC, ATTR1_SIZE32 | facing);
                moveHero(TI2, playerY - 16, playerX+4, offR, offC2, ATTR1_SIZE32 | facing);
            } else {
                moveHero(0, playerY - 16, playerX -4, offR, offC2, ATTR1_SIZE32 | facing);
                moveHero(TI2, playerY - 16, playerX+4, offR, offC, ATTR1_SIZE32 | facing);
            }
        }
    // Not MOVING
    } else {
        moveHero(0, playerY - 16, playerX, offR, offC, ATTR1_SIZE32 | facing);           
    }
    
    
    if (facing == ATTR1_HFLIP) {
        player.hflip = true;
    } else {
        player.hflip = false;
    }
    lastFacing = playerFacing;
}


// Move a "SQUARE_16" sprite
void moveHero(int p, int row, int col, int offR, int offC, int ATTR1) {
    shadowOAM[p].attr0 = (ROWMASK & (row+6 - voff)) | ATTR0_4BPP | ATTR0_REGULAR | ATTR0_TALL; //ATTR0_SQUARE;
    shadowOAM[p].attr1 = (COLMASK & (col - hoff)) | ATTR1; 
    shadowOAM[p].attr2 = SPRITEOFFSET16(offR, offC);
}


// Move a "SQUARE_16" sprite
//void deathHero(int p, int row, int col) {
void dyingPlayer() {
    int row = playerY - 2;
    int col = playerX - 16;
    int hflip = false;
    if (player.hflip == true) { hflip = ATTR1_HFLIP; }
    
    shadowOAM[0].attr0 = (ROWMASK & (row - voff)) | ATTR0_4BPP | ATTR0_REGULAR | ATTR0_SQUARE;
    shadowOAM[0].attr1 = (COLMASK & (col - hoff)) | ATTR1_SIZE32 | hflip; 
    shadowOAM[0].attr2 = SPRITEOFFSET16(0, 21);
    
    shadowOAM[TI2].attr0 = ATTR0_HIDE;
}



// Move a "SQUARE_16" sprite
void moveScorpio(int e, int offR, int offC, int ATTR1) {
    int row = enemies[e].row;
    int col = enemies[e].col;
            
    if (enemies[e].hflip == true) {
        ATTR1 |= ATTR1_HFLIP;
    }
    
    shadowOAM[e].attr0 = (ROWMASK & (row - voff)) | ATTR0_4BPP | ATTR0_REGULAR | ATTR0_WIDE; //ATTR0_SQUARE;
    shadowOAM[e].attr1 = (COLMASK & (col - hoff)) | ATTR1_SIZE32 | ATTR1; 
    shadowOAM[e].attr2 = SPRITEOFFSET16(offR, offC);
}


void moveSoldier1(int e, int offR, int offC, int ATTR1) {
    int row = enemies[e].row;
    int col = enemies[e].col;

    if (enemies[e].hflip == true) {
        ATTR1 |= ATTR1_HFLIP;
    }
    
    int c = 0;
    if (ATTR1 == ATTR1_SIZE64) {
        c = -16;
    }
            
    shadowOAM[e].attr0 = (ROWMASK & (row-20-voff)) | ATTR0_4BPP | ATTR0_REGULAR | ATTR0_SQUARE;
    shadowOAM[e].attr1 = (COLMASK & (col - hoff + c)) | ATTR1; 
    shadowOAM[e].attr2 = SPRITEOFFSET16(offR, offC);
}



void moveThrower1(int e, int offR, int offC, int ATTR1) {
    int row = enemies[e].row;
    int col = enemies[e].col;

    if (enemies[e].hflip == true) {
        ATTR1 |= ATTR1_HFLIP;
    }
                
    shadowOAM[e].attr0 = (ROWMASK & (row-28-voff)) | ATTR0_4BPP | ATTR0_REGULAR | ATTR0_SQUARE;
    shadowOAM[e].attr1 = (COLMASK & (col - hoff)) | ATTR1_SIZE32 | ATTR1; 
    shadowOAM[e].attr2 = SPRITEOFFSET16(offR, offC);
}


void moveKnife(int e, int offR, int offC, int ATTR1) {
    int row = enemies[e].row;
    int col = enemies[e].col;

    if (enemies[e].hflip == true) {
        ATTR1 |= ATTR1_HFLIP;
    }
                
    shadowOAM[e].attr0 = (ROWMASK & (row-voff)) | ATTR0_4BPP | ATTR0_REGULAR | ATTR0_WIDE;
    shadowOAM[e].attr1 = (COLMASK & (col - hoff)) | ATTR1_SIZE8 | ATTR1; 
    shadowOAM[e].attr2 = SPRITEOFFSET16(12, 15);
}




void dyingEnemy(int e) {    
    int ATTR0 = 0;
    int ATTR1 = 0;
    if (enemies[e].hflip == true) {
        ATTR1 = ATTR1_HFLIP;
    }

    int part1Row;
    if (enemies[e].type == SCORPIO) {
        part1Row = 0;
        ATTR0 |= ATTR0_WIDE;
    } else if (enemies[e].type == SOLDIER1) {
        part1Row = 0;
        ATTR0 |= ATTR0_SQUARE;
    }
    
    shadowOAM[e    ].attr0 = (ROWMASK & enemies[e].row+part1Row-voff) | ATTR0_4BPP | ATTR0_REGULAR | ATTR0;
    shadowOAM[e    ].attr1 = (COLMASK & (enemies[e].col-hoff)) | ATTR1_SIZE32 | ATTR1 | ATTR1_VFLIP; 
    shadowOAM[e+TI2].attr0 = (ROWMASK & enemies[e].row-8-voff) | ATTR0_4BPP | ATTR0_REGULAR | ATTR0;
    shadowOAM[e+TI2].attr1 = (COLMASK & (enemies[e].col-hoff)) | ATTR1_SIZE32 | ATTR1 | ATTR1_VFLIP; 
    
    
    enemies[e].row += 2;
    if (enemies[e].row > 168) {
        killEnemy(e);
    }    
}


void killEnemy(int e) {
    enemies[e].alive = DEAD;
    
    //OAM[e].attr0 = (ROWMASK & 160) | ATTR0_HIDE; 
    //OAM[e+100].attr0 = (COLMASK & 240) | ATTR0_HIDE; 
    
    shadowOAM[e].attr0 = (ROWMASK & 160) | ATTR0_HIDE;
    shadowOAM[e].attr1 = (COLMASK & 240);
    shadowOAM[e+TI2].attr0 = (ROWMASK & 160) | ATTR0_HIDE;
    shadowOAM[e+TI2].attr1 = (COLMASK & 240); 
    
    //enemies[e] = enemies[totalEnemies];
    //totalEnemies--;
}


void moveText(int i, int row, int col, int offR, int offC) {
    shadowOAM[i].attr0 = (ROWMASK & row) | ATTR0_4BPP | ATTR0_REGULAR | ATTR0_WIDE;
    shadowOAM[i].attr1 = (COLMASK & col) | ATTR1_SIZE32; 
    shadowOAM[i].attr2 = SPRITEOFFSET16(offR, offC); 
}



void initCarpet(int row, int col, int col2, int t) {
    int c = totalCarpets++;
    
    carpets[c].row = row;
    carpets[c].col = col + rand() % (col2-col);
    carpets[c].col1 = col;
    carpets[c].col2 = col2;
    
    carpets[c].type = t;
    carpets[c].timerMov = 0;
}




// Horizontal walking
void initEnemy(int row, int col, int col2, int t) {
    int e = totalEnemies;
    if (t != KNIFE || enemies[totalEnemies].type != KNIFE) {
        e = ++totalEnemies;
    }
    enemies[e].row = row;
    enemies[e].col = col + rand() % (col2-col);
    enemies[e].col1 = col;
    enemies[e].col2 = col2;
    
    enemies[e].alive = LIVE;
    enemies[e].type = t;
    enemies[e].timerMov = 2500;
}


// Horizontal walking
void moveEnemy(int e) {
    // Hide the enemies we have already walked away
    if (enemies[e].col < hoff - 150 || enemies[e].col > hoff + 450) {
        shadowOAM[e].attr0 = ATTR0_HIDE;
        shadowOAM[e+TI2].attr0 = ATTR0_HIDE;
        return;
    }

    int r = enemies[e].row;
    int cc = enemies[e].col;
    int c1 = enemies[e].col1;
    int c2 = enemies[e].col2;
    //int sp = 4;

    enemies[e].tRow = (r+9)>>3;
    enemies[e].tCol = cc>>3;
    
    
    int checkpointState = 0;
    
    if (enemies[e].type == KNIFE) {
        if (distX(enemies[e].col, enemies[e].col2) > 130) {
            enemies[e].alive = DYING;
            if (enemies[e].timerMov++ > 150) {
                shadowOAM[e].attr0 = ATTR0_HIDE;
                shadowOAM[e+TI2].attr0 = ATTR0_HIDE;
                //killEnemy(e);
            }
        } else {
            // Moving!
            if (enemies[e].hflip) {
                enemies[e].col -= 5;
            } else {
                enemies[e].col += 5;
            }
        }
        moveKnife(e, 15, 12, 0);
    }
    
    // THROWER1
    if (enemies[e].type == THROWER1) {
        int timerMove = enemies[e].timerMov++;
        int CHANGE = 140;
        if (timerMove < CHANGE) checkpointState = 0;
        else checkpointState = 1;
        
        int frame = 0;
        
        switch(checkpointState) {
            case 0: 
                frame = 0;
                break;
            case 1: 
                frame = (timerMove - CHANGE) / 8;
                if (frame > 3) frame = 3;
                // Puts the sound of the knife!
                if (timerMove == CHANGE + 5) {
                    if (distX(enemies[totalEnemies].col, playerX) < 190 && !soundA.isPlaying) {
                        playSoundA(Snd_Knife, SND_KNIFELEN, SND_KNIFEFREQ, 0);
                    }
                } else if (timerMove == CHANGE + 25) {
                    // Create a new KNIFE
                    initEnemy(enemies[e].row-8, enemies[e].col, enemies[e].col, KNIFE);
                    enemies[totalEnemies].hflip = enemies[e].hflip;
                    enemies[totalEnemies].timerMov = 0;
                } else if (timerMove > CHANGE + 50) {
                    enemies[e].timerMov = 0;
                }
                break;
        }

        enemies[    e].hflip = true;
        enemies[e+TI2].hflip = true;
        
        enemies[e+TI2].row = enemies[e].row+16;
        enemies[e+TI2].col = enemies[e].col;

        moveThrower1(    e, 21, frame*4, 0);
        moveThrower1(e+TI2, 23, frame*4, 0);
    }
    // SOLDIER1
    if (enemies[e].type == SOLDIER1) {
        int frame = animationCounter%4;
        
        int timerMove = enemies[e].timerMov++;
        if (timerMove < 1000) checkpointState = 0;
        else if (timerMove < 2000) checkpointState = 1;
        else if (timerMove < 3000) checkpointState = 2;
        else if (timerMove < 5000) checkpointState = 3;
        else checkpointState = 5;
        
        
        switch(checkpointState){
            case 0:
                if (enemies[e].timerMov%2 == 0) enemies[e].col++;
                if (enemies[e].col == enemies[e].col2) {
                    enemies[e].timerMov = 1000;
                }
                enemies[e    ].hflip = true;
                enemies[e+TI2].hflip = true;
                break; 
            case 1: // wait
                if (enemies[e].timerMov%99 == 0) {
                    enemies[e].timerMov = 2000;
                }
                enemies[e    ].hflip = true;
                enemies[e+TI2].hflip = true;
                frame = 0;
                break; 
            case 2: 
                if (enemies[e].timerMov%2 == 0) enemies[e].col--;
                if (enemies[e].col == enemies[e].col1) {
                    enemies[e].timerMov = 3000;
                }
                enemies[e    ].hflip = false;
                enemies[e+TI2].hflip = false;
                break; 
            case 3: // wait
                if (enemies[e].timerMov%99 == 0) {
                    enemies[e].timerMov = 0;
                }
                enemies[e    ].hflip = false;
                enemies[e+TI2].hflip = false;
                frame = 0;
                break;         
        
            case 5:
                enemies[e].timerMov++;
                frame = 3 + (enemies[e].timerMov-5000)/5;
                if (frame >= 8) {
                    frame = 0;
                    //enemies[e].timerMov = 0;
                }
            break;                
        }
        
        int tileC = frame*4;

        int ATTR1 = ATTR1_SIZE32;
        if (frame == 5) { ATTR1 = ATTR1_SIZE64; }
        if (frame == 6) { tileC += 4; }

        enemies[e+TI2].row = enemies[e].row+8;
        enemies[e+TI2].col = enemies[e].col;

        moveSoldier1(    e, 16, tileC, ATTR1);
        moveSoldier1(e+TI2, 17, tileC, ATTR1);

        /*
        // ATTACK
        if (enemies[e].timerMov >= 5000) {
            enemies[e].timerMov++;
            frame = 3 + (enemies[e].timerMov-5000)/5;
            if (frame >= 8) {
                frame = 0;
                //enemies[e].timerMov = 0;
            }
        }
        int tileC = frame*4;
        
        int ATTR1 = ATTR1_SIZE32;
        if (frame == 5) { ATTR1 = ATTR1_SIZE64; }
        if (frame == 6) { tileC += 4; }
        
        enemies[e+TI2].row = enemies[e].row+8;
        enemies[e+TI2].col = enemies[e].col;
        
        moveSoldier1(    e, 16, tileC, ATTR1);
        moveSoldier1(e+TI2, 17, tileC, ATTR1);
        */
    }
    
    // SCORPIO
    if (enemies[e].type == SCORPIO) {     
        int timerMove = enemies[e].timerMov++;
        if (timerMove < 1000) checkpointState = 0;
        else if (timerMove < 2000) checkpointState = 1;
        else if (timerMove < 3000) checkpointState = 2;
        else checkpointState = 3;
        
        int tileC = 0;
        enemies[e+TI2].row = enemies[e].row+8;
        enemies[e+TI2].col = enemies[e].col;
        
        switch(checkpointState){
            case 0:
                if (enemies[e].timerMov%2 == 0) enemies[e].col++;
                if (enemies[e].col == enemies[e].col2) {
                    enemies[e].timerMov = 1000;
                }
                tileC = (animationCounter%3)*4;
                enemies[e    ].hflip = true;
                enemies[e+TI2].hflip = true;
                moveScorpio(    e, 13, tileC, 0);
                moveScorpio(e+TI2, 14, tileC, 0);
                
                if (distX(enemies[e].col, playerX) < 60 && !soundA.isPlaying) {
                    playSoundA(Snd_Scorpio, SND_SCORPIOLEN, SND_SCORPIOFREQ, 0);
                }
                break; 
            case 1: // wait
                if (enemies[e].timerMov%99 == 0) {
                    enemies[e].timerMov = 2000;
                }
                enemies[e    ].hflip = true;
                enemies[e+TI2].hflip = true;
                moveScorpio(    e, 13, tileC, 0);
                moveScorpio(e+TI2, 14, tileC, 0);
                break; 
            case 2: 
                if (enemies[e].timerMov%2 == 0) enemies[e].col--;
                if (enemies[e].col == enemies[e].col1) {
                    enemies[e].timerMov = 3000;
                }
                tileC = (animationCounter%3)*4;
                enemies[e    ].hflip = false;
                enemies[e+TI2].hflip = false;
                moveScorpio(    e, 13, tileC, 0);
                moveScorpio(e+TI2, 14, tileC, 0);
                
                if (distX(enemies[e].col, playerX) < 60 && !soundA.isPlaying) {
                    playSoundA(Snd_Scorpio, SND_SCORPIOLEN, SND_SCORPIOFREQ, 0);
                }
                break; 
            case 3: // wait
                if (enemies[e].timerMov%99 == 0) {
                    enemies[e].timerMov = 0;
                }
                enemies[e    ].hflip = false;
                enemies[e+TI2].hflip = false;
                moveScorpio(    e, 13, tileC, 0);
                moveScorpio(e+TI2, 14, tileC, 0);
                break;         
        }
    }
}




void putLifes(int lifes) {
    // Silouette
    shadowOAM[TLI].attr0 = (ROWMASK & 1) | ATTR0_4BPP | ATTR0_REGULAR | ATTR0_TALL; //ATTR0_SQUARE;
    shadowOAM[TLI].attr1 = (COLMASK & 3) | ATTR1_SIZE32; 
    shadowOAM[TLI].attr2 = SPRITEOFFSET16(0, 25);
    
    // X
    shadowOAM[TLI+1].attr0 = (ROWMASK & 10) | ATTR0_4BPP | ATTR0_REGULAR | ATTR0_SQUARE;
    shadowOAM[TLI+1].attr1 = (COLMASK & 20) | ATTR1_SIZE8; 
    shadowOAM[TLI+1].attr2 = SPRITEOFFSET16(12, 14);

    // Numbers
    int n[2];
    n[1] = lifes/10;
    n[0] = lifes%10;
    int nums = 2;
    if (n[1] == 0) nums = 1;
    
    int offR, offC;
    int i;
    int digit;
    for (i = 0; i < nums; i++) {
        digit = n[nums-1-i]; 
        if (digit < 5) {
            offR = 8; offC = 14 + 2*digit;
        } else {
            offR = 10; offC = 14+2*(digit-5);
        }
        shadowOAM[TLI+2+i].attr0 = (ROWMASK & 5) | ATTR0_4BPP | ATTR0_REGULAR | ATTR0_SQUARE;
        shadowOAM[TLI+2+i].attr1 = (COLMASK & (26+i*15)) | ATTR1_SIZE16; 
        shadowOAM[TLI+2+i].attr2 = SPRITEOFFSET16(offR, offC);    
    }
}



void putMyCoins(const int myCoins) {
    // Diamond
    shadowOAM[TLI+4].attr0 = (ROWMASK & 4) | ATTR0_4BPP | ATTR0_REGULAR | ATTR0_SQUARE;
    shadowOAM[TLI+4].attr1 = (COLMASK & 184) | ATTR1_SIZE16; 
    shadowOAM[TLI+4].attr2 = SPRITEOFFSET16(0, 27);
    
    // X
    shadowOAM[TLI+5].attr0 = (ROWMASK & 10) | ATTR0_4BPP | ATTR0_REGULAR | ATTR0_SQUARE;
    shadowOAM[TLI+5].attr1 = (COLMASK & 200) | ATTR1_SIZE8; 
    shadowOAM[TLI+5].attr2 = SPRITEOFFSET16(12, 14);

    // Numbers
    int n[2];
    n[1] = myCoins/10;
    n[0] = myCoins%10;

    int offR, offC;
    int digit;
    // Only 1 digit
    if (n[1] == 0) {
        digit = n[0];
        if (digit < 5) {
            offR = 8; offC = 14 + 2*digit;
        } else {
            offR = 10; offC = 14+2*(digit-5);
        }
        shadowOAM[TLI+7].attr0 = ATTR0_HIDE;
        shadowOAM[TLI+6].attr0 = (ROWMASK & 5) | ATTR0_4BPP | ATTR0_REGULAR | ATTR0_SQUARE;
        shadowOAM[TLI+6].attr1 = (COLMASK & (207)) | ATTR1_SIZE16; 
        shadowOAM[TLI+6].attr2 = SPRITEOFFSET16(offR, offC);    
    // 2 digits...
    } else {
        digit = n[1];
        if (digit < 5) {
            offR = 8; offC = 14 + 2*digit;
        } else {
            offR = 10; offC = 14+2*(digit-5);
        }
        shadowOAM[TLI+6].attr0 = (ROWMASK & 5) | ATTR0_4BPP | ATTR0_REGULAR | ATTR0_SQUARE;
        shadowOAM[TLI+6].attr1 = (COLMASK & (207)) | ATTR1_SIZE16; 
        shadowOAM[TLI+6].attr2 = SPRITEOFFSET16(offR, offC);            

        digit = n[0];
        if (digit < 5) {
            offR = 8; offC = 14 + 2*digit;
        } else {
            offR = 10; offC = 14+2*(digit-5);
        }
        shadowOAM[TLI+7].attr0 = (ROWMASK & 5) | ATTR0_4BPP | ATTR0_REGULAR | ATTR0_SQUARE;
        shadowOAM[TLI+7].attr1 = (COLMASK & (207+12)) | ATTR1_SIZE16; 
        shadowOAM[TLI+7].attr2 = SPRITEOFFSET16(offR, offC);            
    }
}


int checkCollisions() {
    int DEAD_Y;
    int DEAD_X;
    int START_X = 0;
    
    int i;
    int enX, enY;
 
    // CHEAT makes you INVULNERABLE
    //if (cheatInvulnerable == 0) {
    for (i = 1; i <= totalEnemies; i++) {
        if (enemies[i].alive != LIVE) continue;
        //if (enemies[i].type == THROWER1) continue;

        enX = enemies[i].col;
        enY = enemies[i].row;

        switch(enemies[i].type) {
            case KNIFE:
                DEAD_Y = 24;
                DEAD_X = 8+4;
                START_X = enX;
                break;
            case THROWER1:
                DEAD_Y = 48;
                DEAD_X = 8+4;
                START_X = enX;
            case SCORPIO: 
                DEAD_Y = 16;
                DEAD_X = 8+4;
                START_X = enX+8;
                //if (enemies[i].hflip == 1) { START_X = enX ; }
                //else { START_X = enX; }
                break;
            case SOLDIER1:
                DEAD_Y = 64;
                DEAD_X = 16+4;
                START_X = enX+12;
                //if (enemies[i].hflip == 1) { START_X = enX; }
                //else { START_X = enX; }
        }

        //if (playerY > enY - DEAD_Y && playerY < enY + DEAD_Y) {
        if (distX(START_X, playerX) < DEAD_X) {
        //if (playerX > enX - DEAD_X && playerX < enX + DEAD_X) {

            if ((playerY <= enY + 8) && (distY(enY, playerY) < DEAD_Y)) {
                // IF... JUMPING...(falling)
                if (enemies[i].type != KNIFE && isJumping && jumpInc > 1 && (enY - playerY > DEAD_Y*0.5) ) {
                    //killEnemy(i);
                    enemies[i].alive = DYING;

                    if (enemies[i].type == SCORPIO) {
                        playSoundA(powerupsfx, POWERUPSFXLEN, POWERUPSFXFREQ, 0);
                        putCoins(enY+5, enX, 6);
                    } else if (enemies[i].type == SOLDIER1) {
                        playSoundA(powerupsfx, POWERUPSFXLEN, POWERUPSFXFREQ, 0);
                        putCoins(enY+5, enX, 10);
                    }
                } else if (enemies[i].type == THROWER1 ) {
                    // Thrower1 -- Doesn't kill you
                } else if (cheatInvulnerable) {
                    // NO ONE CAN KILL YOU
                } else {
                    //collisions = 1;   //player collided with an Enemy --> DIE
                    player.alive = DYING;
                    if (isJumping) {
                        playerY = enemies[i].row;
                        //playerY = jumpY;
                    }

                    if (enemies[i].type == SOLDIER1) {
                        enemies[i    ].hflip = false;
                        enemies[i+TI2].hflip = false;
                        if (enemies[i].col < playerX) {
                            enemies[i    ].hflip = true;
                            enemies[i+TI2].hflip = true;
                        }

                        enemies[i].timerMov = 5000;
                        playSoundA(Snd_Sword, SND_SWORDLEN, SND_SWORDFREQ, 0);
                    }
                    //return 1;
                }
            }
        }
    }
    //}
    
    // Get Coins!!
    int coY, coX;
    for (i = 0; i < totalCoins; i++) {
        coY = coins[i].row;
        coX = coins[i].col;
        
        if (distX(coX, playerX) < 7 && distY(coY, playerY) < 7) {
            myCoins += coins[i].timerMov;
            // If 50 coins, get one life!!! :D
            if (myCoins >= 100) {
                myCoins -= 100;
                lifes++;
                playSoundA(powerupsfx, POWERUPSFXLEN, POWERUPSFXFREQ, 0);
            }
            
            coins[i] = coins[totalCoins-1];
            shadowOAM[TCO+totalCoins-1].attr0 = ATTR0_HIDE;        
            totalCoins--;
        }
    }
    
    return 0;
}


int distY(int y1, int y2) {
    int tmp = y1-y2;
    if (tmp < 0) tmp *= -1;
    return tmp;
}

int distX(int x1, int x2) {
    int tmp = x1-x2;
    if (tmp < 0) tmp *= -1;
    return tmp;
}


void putCoins(int row, int col, int max) {
    int i;
    //int total = 2 + rand() % max;
    int total = 3;
    
    for (i = 0; i < total; i++) {
        coins[totalCoins].row = row-38;
        coins[totalCoins].col = col-8 + rand()%16;
 
        coins[totalCoins].col2 = row-4;
        
        coins[totalCoins].timerMov = 1 + rand() % (max/3+2);
        totalCoins++;        
    }
}

void showCP() {
    int row, col;
    int i;
    
    shadowOAM[TCP].attr0 = ATTR0_HIDE;
    shadowOAM[TCP+1].attr0 = ATTR0_HIDE;
    
    // SOLO DIBUJAR MAX. 1!!!!
    for (i = 0; i < totalCP; i++) {
        row = 115;
        col = checkPoints[i];
        
        if (col > hoff - 16 && col < hoff + 248) {
            shadowOAM[TCP].attr0 = (ROWMASK & (row-voff)) | ATTR0_4BPP | ATTR0_REGULAR | ATTR0_TALL;
            shadowOAM[TCP].attr1 = (COLMASK & (col-hoff)) | ATTR1_SIZE32; 
            shadowOAM[TCP].attr2 = SPRITEOFFSET16(0, 29);

            shadowOAM[TCP+1].attr0 = (ROWMASK & (row-voff)) | ATTR0_4BPP | ATTR0_REGULAR | ATTR0_TALL;
            shadowOAM[TCP+1].attr1 = (COLMASK & (col-hoff+8)) | ATTR1_SIZE32; 
            shadowOAM[TCP+1].attr2 = SPRITEOFFSET16(0, 30);
            return;
        }
    }
}


void showCoins() {
    coinsTimer++;
    int flip = 0;
    if (coinsTimer > 30) {
        flip = ATTR1_HFLIP;
        if (coinsTimer > 60) {
            coinsTimer = 0;
        }
    }
    
    int i;
    for (i = 0; i < 15; i++) {
        shadowOAM[TCO+i].attr0 = ATTR0_HIDE;
    }
    
        
    int row, col;
    int coinsShown = 0;
    for (i = 0; i < totalCoins; i++) {
        row = coins[i].row;
        col = coins[i].col;
        
        // Move coins down!
        if (row < coins[i].col2) coins[i].row++;
        
        // Only coins NOW in screen coord.
        if (col > hoff - 24 && col < hoff + 256) {
            shadowOAM[TCO+coinsShown].attr0 = (ROWMASK & (row-voff)) | ATTR0_4BPP | ATTR0_REGULAR | ATTR0_SQUARE;
            shadowOAM[TCO+coinsShown].attr1 = (COLMASK & (col-hoff)) | ATTR1_SIZE16 | flip; 
            shadowOAM[TCO+coinsShown].attr2 = SPRITEOFFSET16(0, 27);       
            coinsShown++;
        }
    }
}



#define NUU 1

#ifdef NUU
void showCarpets() {
    int i;
    for (i = 0; i < 10; i++) {
        shadowOAM[TCA+i].attr0 = ATTR0_HIDE;
    }
    
    const int TIMER_FRAME = 10;
    int timerMov;
    int frame;
    int row, col;
    int carpetsShown = 0;
    for (i = 0; i < totalCarpets; i++) {
        timerMov = carpets[i].timerMov++;
        // Choose the frame
        timerMov %= 40;
        frame = timerMov / 10;
        
        // Move the carpet left or right
        if (carpets[i].timerMov < 1000) { // LEFT
            carpets[i].col--;
            if (carpets[i].col <= carpets[i].col1) {
                carpets[i].timerMov = 1000;
            }
        } else if (carpets[i].timerMov >= 1000) { // RIGHT
            carpets[i].col++;
            if (carpets[i].col >= carpets[i].col2) {
                carpets[i].timerMov = 0;
            }
        }
        
        row = carpets[i].row;
        col = carpets[i].col;
                
        // Only carpets NOW in screen coord.
        if (col > hoff - 64 && col < hoff + 256) {
            shadowOAM[TCA+carpetsShown*2].attr0 = (ROWMASK & (row-voff)) | ATTR0_4BPP | ATTR0_REGULAR | ATTR0_WIDE;
            shadowOAM[TCA+carpetsShown*2].attr1 = (COLMASK & (col-hoff)) | ATTR1_SIZE32; 
            shadowOAM[TCA+carpetsShown*2].attr2 = SPRITEOFFSET16(30, frame*8);       
            
            shadowOAM[TCA+carpetsShown*2+1].attr0 = (ROWMASK & (row-voff)) | ATTR0_4BPP | ATTR0_REGULAR | ATTR0_WIDE;
            shadowOAM[TCA+carpetsShown*2+1].attr1 = (COLMASK & (col-hoff+32)) | ATTR1_SIZE32; 
            shadowOAM[TCA+carpetsShown*2+1].attr2 = SPRITEOFFSET16(30, frame*8+4);       
            carpetsShown++;
        }
    }
}
#endif

