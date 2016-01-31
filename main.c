/***************************************** 
 * TITLE: Bagdad's Thief 
 * AUTHOR: Jay Sueksagan
******************************************/
/***************************************** 
I am making a platform game where the player
has to reach the goal line at the end of
a long scrolling map.

The player can run, jump, frontflips and kill
the enemies jumping on them. Also the enemies
can kill the player.

There are four kinds of enemies. The game has
"coins", when getting 100 of them, the player
gets one more life. There are also CheckPoints
to save player's position in case you die. The
game also has moving platforms (magic carpets).
 
There are two tiled backgrounds which scroll
with different speeds creating a Parallax
effect of depth.
******************************************/
 
#include <stdio.h>
#include "myLib.h"
#include "levels.h"
#include "game.h"
#include "sound.h"

// Load screens
#include "images/ScreenStart.h"
#include "images/GameOver.h"
#include "images/Papyrus.h"
#include "images/PapyrusStory1.h"
#include "images/PapyrusStory2.h"
#include "images/PapyrusStory3.h"
#include "images/Controls.h"
#include "images/Level1.h"
#include "images/PauseScreen.h"
#include "images/TheEnd.h"
// Load sounds
#include "sound/Snd_Start.h"
#include "sound/Snd_Level1.h"
#include "sound/powerupsfx.h"
#include "sound/Snd_GameOver.h"
#include "sound/Snd_TheEnd.h"

#define PLAY_MUSIC 1

//#include "more.h"

#define MAXWIDTH 239
#define MAXHEIGHT 159

#define STARTSCREEN 0
#define MENUSCREEN 10
#define MCREDITSSCREEN 11
#define MCONTROLSSCREEN 12
#define MINSTRUCTIONSSCREEN 13
#define GAMESCREEN 1
#define LOSESCREEN 2
#define WINSCREEN 3
#define ENDCREDITSSCREEN 31
#define PAUSESCREEN 4
#define GAMEOVERSCREEN 5

int state;
int timerState;         // Timer to make the texts "Press Start Button" blink
int timerDead;          // Time to wait when the player dies
// Menus
int optionMenu = 0;
int *menuOptions[4] = { "PLAY", "INSTRUCTIONS", "CONTROLS", "CREDITS" };
int totalOptions = 4;
// Menu Font Colors
int COL_SHADOW = BLACK;
int COL_TEXT = BLECK;
int COL_TITLE = BLECK1;
int COL_SELECTED = RED;
// GameOver
int gameOverTopRow;


/* Game Vars */
int level;

/* Functions */
void start();
void menu();
void menuCredits();
void menuControls();
void menuInstructions();
void game();
void pause();
void win();
void lose();
void gameOver();
void drawCredits();

void initGameVars();


void initGameVars() {
    cheatInvulnerable = 0;
    lifes = 3;
    checkPoint = -1;
    level = 1;
    myCoins = 0;
}

    


/* Main program */
int main() {   
    if (PLAY_MUSIC == 1) {
        startSounds();
    }

    SETMODE(MODE3 | BG2_ENABLE);
    state = STARTSCREEN;
    timerState = 0;
    drawImage(ScreenStartBitmap, 0, 0, 160, 240);
    playSoundB(Snd_Start, SND_STARTLEN, SND_STARTFREQ, 1);
    
    
    
    /* Game loop */
    while(1) {
        switch(state) {
            case STARTSCREEN:
                start();
                break;
            case MENUSCREEN:
                menu();
                break;
            case MCREDITSSCREEN:
                menuCredits();
                break;
            case MCONTROLSSCREEN:
                menuControls();
                break;
            case MINSTRUCTIONSSCREEN:
                menuInstructions();
                break;
            case GAMESCREEN:
                game();
                break;
            case PAUSESCREEN:
                pause();
                break;
            case WINSCREEN:
                win();
                break;
            case ENDCREDITSSCREEN:
                endCredits();
                break;
            case LOSESCREEN:
                lose();
                break;
            case GAMEOVERSCREEN:
                gameOver();
                break;
            default: 
                start();
                break;
        }
        
        if (state != GAMESCREEN) {
            waitForVblank();
        }
    }
        

    return 0;
}



void start() {
    // Show Press Start (blinking)
    blinkPressStart(ScreenStartBitmap);
    
    if(KEY_DOWN_NOW(BUTTON_START)) {
        while(KEY_DOWN_NOW(BUTTON_START));
        state = MENUSCREEN;
        fillScreen3(BLACK);
        drawImage(PapyrusBitmap, 0, 0, 160, 240);
        int i;
        for (i = 0; i < totalOptions; i++) {
            //drawString(37+i*25, 120, menuOptions[i], COL_SHADOW);
            //drawString(36+i*25, 120, menuOptions[i], COL_TEXT);
            drawString(30+i*25, 120, menuOptions[i], COL_SHADOW);
            drawString(29+i*25, 120, menuOptions[i], COL_TEXT);
        }
    }

}


void blinkPressStart(const unsigned short* bitmap) {
    timerState++;
    if (timerState == 2) {
        drawString(127,120, "Press START", BLECK);
        drawString(126,120, "Press START", WHITE);
    } else if (timerState == 120) {
        drawImage(bitmap, 0, 0, 160, 240);
    } else if (timerState == 150){
        timerState = 0;
    }

}


void menu() {
    int change = false;
    int oldOption = optionMenu;
    int i = 0;
    
    // Write the current option
    drawString(29+optionMenu*25, 120, menuOptions[optionMenu], COL_SELECTED);
    drawString(29+optionMenu*25, 119, menuOptions[optionMenu], COL_SELECTED);
    
    // Read buttons to change the optionMenu
    if(KEY_DOWN_NOW(BUTTON_DOWN) && optionMenu < (totalOptions-1)) {
        while(KEY_DOWN_NOW(BUTTON_DOWN));
        optionMenu++;
        change = true;
    }
    if(KEY_DOWN_NOW(BUTTON_UP) && optionMenu > 0) {
        while(KEY_DOWN_NOW(BUTTON_UP));
        optionMenu--;
        change = true;
    } 

    if (change) {
        // Redraw background and "Black" options
        drawImage(PapyrusBitmap, 0, 0, 160, 240);
        for (i = 0; i < totalOptions; i++) {
            drawString(30+i*25, 120, menuOptions[i], COL_SHADOW);
            drawString(29+i*25, 120, menuOptions[i], COL_TEXT);
        }
    }
    
    
    if(KEY_DOWN_NOW(BUTTON_START)) {
        while(KEY_DOWN_NOW(BUTTON_START));
        
        switch(optionMenu) {
            case 0: // PLAY
                state = GAMESCREEN;
                 fillScreen3(BLACK);
                initGameVars();
                loadLevel(level);
                initSprites();
                // Play looping sound
                playSoundB(Snd_Level1, SND_LEVEL1LEN, SND_LEVEL1FREQ, 1);
                break;
            case 1: // INSTRUCTIONS
                state = MINSTRUCTIONSSCREEN;
                fillScreen3(BLACK);
                drawImage(PapyrusStory1Bitmap, 0, 0, 160, 240);                
                timerState = 0;                
                break;
            case 2: // CONTROLS
                state = MCONTROLSSCREEN;
                fillScreen3(BLACK);
                drawImage(ControlsBitmap, 0, 0, 160, 240);
                drawString(27, 120, "CONTROLS", COL_SHADOW);
                drawString(26, 120, "CONTROLS", COL_TITLE);
                
                drawString(72, 61, "move", COL_SHADOW);
                drawString(72, 179, "jump", COL_SHADOW);
                drawString(71, 61, "move", COL_TEXT);
                drawString(71, 179, "jump", COL_TEXT);                

                drawString(106, 120, "Left,Right Move", COL_SHADOW);
                drawString(121, 120, "Button A   Jump", COL_SHADOW);
                drawString(105, 120, "Left,Right Move", COL_TITLE);
                drawString(120, 120, "Button A   Jump", COL_TITLE);
                break;
            case 3: // CREDITS
                state = MCREDITSSCREEN;
                fillScreen3(BLACK);
                drawImage(PapyrusBitmap, 0, 0, 160, 240);
                drawString(27, 120, "CREDITS", COL_SHADOW);
                drawString(26, 120, "CREDITS", COL_TITLE);
                drawString(65, 120, "Jay Sueksagan", COL_SHADOW);
                drawString(64, 120, "Jay Sueksagan", COL_TEXT);
                drawString(64, 120, "Jay Sueksagan", COL_TEXT);
                drawString(78, 120, "2012 0", COL_SHADOW);
                drawString(77, 120, "2012 0", COL_TEXT);
                drawString(79, 141, "c", COL_TEXT);
                break;
            default:
                break;
        }
   }

}



void menuCredits() {
    // Show who's your daddy ;)
    if(KEY_DOWN_NOW(BUTTON_START)) {
        while(KEY_DOWN_NOW(BUTTON_START));
        state = MENUSCREEN;
        fillScreen3(BLACK);
        drawImage(PapyrusBitmap, 0, 0, 160, 240);
        int i;
        for (i = 0; i < totalOptions; i++) {
            drawString(30+i*25, 120, menuOptions[i], COL_SHADOW);
            drawString(29+i*25, 120, menuOptions[i], COL_TEXT);
        }
    }
}



void menuControls() {
    if(KEY_DOWN_NOW(BUTTON_START)) {
        while(KEY_DOWN_NOW(BUTTON_START));
        state = MENUSCREEN;
        fillScreen3(BLACK);
        drawImage(PapyrusBitmap, 0, 0, 160, 240);
        int i;
        for (i = 0; i < totalOptions; i++) {
            drawString(30+i*25, 120, menuOptions[i], COL_SHADOW);
            drawString(29+i*25, 120, menuOptions[i], COL_TEXT);
        }
    }
}


void menuInstructions() {
    //timerState++;
        
    int COL = GREY;
    if (timerState == 0) {
        drawImage(PapyrusStory1Bitmap, 0, 0, 160, 240);                
        drawString(106, 120, "The wondrous city", COL);
        drawString(121, 120, "of Bagdad is...", COL);
        drawString(105, 120, "The wondrous city", COL_TEXT);
        drawString(120, 120, "of Bagdad is...", COL_TEXT);                
        timerState++;
    } else if (timerState == 2) {
        drawImage(PapyrusStory1Bitmap, 0, 0, 160, 240);                
        drawString(106, 118, "Full of magic and", COL);
        drawString(121, 119, "gold. Ahmed is a", COL);
        drawString(105, 118, "Full of magic and", COL_TEXT);
        drawString(120, 119, "gold. Ahmed is a", COL_TEXT);                
        timerState++;
    } else if (timerState == 4) {
        drawImage(PapyrusStory2Bitmap, 0, 0, 160, 240);                
        drawString(106, 119, "treasure hunter.", COL);
        drawString(121, 119, "He will live the", COL);
        drawString(105, 119, "treasure hunter,", COL_TEXT);
        drawString(120, 119, "He will live the", COL_TEXT);                
        timerState++;
    } else if (timerState == 6) {
        drawImage(PapyrusStory2Bitmap, 0, 0, 160, 240);                
        drawString(106, 119, "best adventures,", COL);
        drawString(121, 119, "escape the guards,", COL);
        drawString(105, 119, "best adventures,", COL_TEXT);
        drawString(120, 119, "escape the guards,", COL_TEXT);                
        timerState++;
    } else if (timerState == 8) {
        drawImage(PapyrusStory3Bitmap, 0, 0, 160, 240);                
        drawString(106, 119, "meet a beautiful", COL);
        drawString(121, 120, "princess andFight", COL);
        drawString(105, 119, "meet a beautiful", COL_TEXT);
        drawString(120, 120, "princess andFight", COL_TEXT);                
        timerState++;
    } else if (timerState == 10) {
        drawImage(PapyrusStory3Bitmap, 0, 0, 160, 240);                
        drawString(106, 119, "the evil sultane", COL);
        drawString(121, 119, "becoming a hero...", COL);
        drawString(105, 119, "the evil sultane", COL_TEXT);
        drawString(120, 119, "becoming a hero...", COL_TEXT);                
        timerState++;
    }
    
    if (KEY_DOWN_NOW(BUTTON_A)) {
        while(KEY_DOWN_NOW(BUTTON_A));
        timerState++;
    }
    if (KEY_DOWN_NOW(BUTTON_B)) {
        while(KEY_DOWN_NOW(BUTTON_B));
        timerState++;
    }
    if (KEY_DOWN_NOW(BUTTON_START)) {
        while(KEY_DOWN_NOW(BUTTON_START));
        timerState++;
    }

    
    int goBack = false;
    if(KEY_DOWN_NOW(BUTTON_START)) {
        while(KEY_DOWN_NOW(BUTTON_START));
        //goBack = true;
    }
    
    if (goBack || timerState >= 12) {
        state = MENUSCREEN;
        fillScreen3(BLACK);
        drawImage(PapyrusBitmap, 0, 0, 160, 240);
        int i;
        for (i = 0; i < totalOptions; i++) {
            drawString(30+i*25, 120, menuOptions[i], COL_SHADOW);
            drawString(29+i*25, 120, menuOptions[i], COL_TEXT);
        }
    }
}



void game() { 
    int incX = 2;

    if (player.alive == LIVE) {
        /* Read buttons */
        if(KEY_DOWN_NOW(BUTTON_LEFT)) {
            if (playerX > 0) {
                playerX -= incX;
            }
        }
        if(KEY_DOWN_NOW(BUTTON_RIGHT)) {
            playerX += incX;
        }
        if(KEY_DOWN_NOW(BUTTON_UP)) {
            //voff--;
        }
        if(KEY_DOWN_NOW(BUTTON_DOWN)) {
            //voff++;
        }
        if(KEY_DOWN_NOW(BUTTON_A)) { // Jump
            if (isJumping == 0 && canJump == 1) {
                //playSoundA(powerupsfx, POWERUPSFXLEN, POWERUPSFXFREQ, 0);
                isJumping = true;
                jumpInc = -MAX_JUMP;
                timerJump = 10;
                jumpY = playerY;
            }
        }
        if(KEY_DOWN_NOW(BUTTON_L) && KEY_DOWN_NOW(BUTTON_R)) { // Jump
            cheatInvulnerable = 1;
        }
    } else {
        timerDead++;
        if (timerDead >= 200) {
            timerDead = 0;
            REG_DISPCTL = MODE3 | BG2_ENABLE;
            state = LOSESCREEN;
            hoff = 0;
            voff = 0;
            lifes--;
            fillScreen3(BLACK);
            
            if (lifes <= 0) {
                SETMODE(MODE3 | BG2_ENABLE);
                state = GAMEOVERSCREEN;
                timerState = 0;
                gameOverTopRow = 140*3;   //160*2
                playSoundB(Snd_GameOver, SND_GAMEOVERLEN, SND_GAMEOVERFREQ, 1);
            }            
        }
    }
    
    // PAUSE the game, only if player is alive!
    if (player.alive == LIVE) {
        if(KEY_DOWN_NOW(BUTTON_START)) {
            while(KEY_DOWN_NOW(BUTTON_START));
            stopSoundB();
            playSoundA(powerupsfx, POWERUPSFXLEN, POWERUPSFXFREQ, 0);
            state = PAUSESCREEN;
            timerState = 0;
            REG_DISPCTL = MODE3 | BG2_ENABLE;
            fillScreen3(BLACK);
            drawImage(PauseScreenBitmap, 0, 0, 160, 240);
            drawString(60, 120, "PAUSE", WHITE);    
        }
    }
    /* End of read buttons */

    // Check win
    // WIN?
    //if (hoff >= levelWidth() - 256) {
    if (playerX >= levelWidth() - 40) {
        REG_DISPCTL = MODE3 | BG2_ENABLE;
        state = WINSCREEN;
        fillScreen3(BLACK);
        timerState = 0;
        //drawImage(TheEndBitmap, 0, 0, 160, 240);
        playSoundB(Snd_TheEnd, SND_THEENDLEN, SND_THEENDFREQ, 1);
    }
    // End of check win
    
    //------Draw-------
    waitForVblank();
    
    loopGame(lifes, myCoins);
}



void pause() {       
    blinkPressStart(PauseScreenBitmap);
    
    if (timerState == 120) {
        drawString(60, 120, "PAUSE", WHITE);
    }
    
    // Wait until "START" pressed
    if(KEY_DOWN_NOW(BUTTON_START)) {
        while(KEY_DOWN_NOW(BUTTON_START));
                
        state = GAMESCREEN;
        fillScreen3(BLACK);
                
        showLevel(level);
        loadSprites();        
        small_hoff = hoff%8;        
        loadMapBlockCol(hoff/8, Level1Map);
        
        // Play looping sound
        playSoundB(Snd_Level1, SND_LEVEL1LEN, SND_LEVEL1FREQ, 1);
    }
}

void win() {
    timerState++;
    
    if (timerState >= 100 && timerState <= 350) { //350
        drawImage(TheEndBitmap, 0, 0, 160, 240);
        // Now the BLACK parts:
        drawRect3(timerState-100, 0, 160, 240, BLACK);
    }

    if (timerState < 200) return;

    
    int credits = false;
    
    // Wait until "START" button is pressed
    if(KEY_DOWN_NOW(BUTTON_START)) {
        while(KEY_DOWN_NOW(BUTTON_START));
        credits = true;
    }
    
    if (credits || timerState > 350) {
        SETMODE(MODE3 | BG2_ENABLE);
        state = ENDCREDITSSCREEN;
        timerState = 0;
        fillScreen3(BLACK);
    }
}



void endCredits() {
    timerState++;

    if (timerState < 50) {
        drawString(50, 120, "THE THIEF", WHITE);
        drawString(70, 120, "OF BAGDAD", WHITE);
    } else if (timerState == 50) {
        fillScreen3(BLACK);
    } else if (timerState > 50 && timerState < 100) {
        drawString(25, 120, "A game by", WHITE);
        drawString(60, 120, "Jay", WHITE);
        drawString(80, 120, "Sueksagan", WHITE);
    } else if (timerState == 100) {
        fillScreen3(BLACK);
    } else if (timerState > 100 && timerState < 200) {
        drawString(60, 120, "Special thanks", WHITE);
    } else if (timerState == 200) {
        fillScreen3(BLACK);
    } else if (timerState > 200 && timerState < 250) {
        drawString(60, 120, "DISNEYs Aladdin", WHITE);
    } else if (timerState == 250) {
        fillScreen3(BLACK);
    } else if (timerState > 250 && timerState < 300) {
        drawString(60, 120, "The Thief of Bagdad", WHITE);
    } else if (timerState == 300) {
        fillScreen3(BLACK);
    } else if (timerState > 300 && timerState < 350) {
        drawString(60, 120, "My mum and family", WHITE);
    } else if (timerState == 350) {
        fillScreen3(BLACK);
    } else if (timerState > 350 && timerState < 400) {
        drawString(60, 120, "And my friends", WHITE);
    } else if (timerState == 400) {
        fillScreen3(BLACK);
    } else if (timerState > 450) {
        drawString(50, 120, "THE THIEF", WHITE);
        drawString(70, 120, "OF BAGDAD", WHITE);
    }
    
    int quitCredits = false;
    
    // Wait until "START" button is pressed
    if(KEY_DOWN_NOW(BUTTON_START)) {
        while(KEY_DOWN_NOW(BUTTON_START));
        quitCredits = true;
    }
    
    if ((timerState > 100 && quitCredits) || timerState > 650) {
        SETMODE(MODE3 | BG2_ENABLE);
        state = STARTSCREEN;
        timerState = 0;
        drawImage(ScreenStartBitmap, 0, 0, 160, 240);
        playSoundB(Snd_Start, SND_STARTLEN, SND_STARTFREQ, 1);

    }
}



void lose() {
    /*
    drawString(50,60, "TRY AGAIN", RED);
    drawString(70,50, "Press START", WHITE);
    
    if(KEY_DOWN_NOW(BUTTON_START)) {
        while(KEY_DOWN_NOW(BUTTON_START));
    */          
        state = GAMESCREEN;
        fillScreen3(BLACK);
        //SETMODE(MODE0 | BG0_ENABLE | /*BG1_ENABLE |*/ BG2_ENABLE | SPRITE_ENABLE);
        
        loadLevel(level);
        initSprites();
        
    //}
}



void gameOver() {
    if (gameOverTopRow > 75 /*25*3*/) {
        gameOverTopRow -= 2;

        drawImage(GameOverBitmap, 0, 0, 160, 240);
        // Now the BLACK parts:        
        drawRect3(0, 0, gameOverTopRow>>2, 240, BLACK);

        return;
    }        
    
    // Blink "Press Start"
    blinkPressStart(GameOverBitmap);
    
    // Wait until "START" pressed
    if(KEY_DOWN_NOW(BUTTON_START)) {
        while(KEY_DOWN_NOW(BUTTON_START));
        
        SETMODE(MODE3 | BG2_ENABLE);
        state = STARTSCREEN;
        timerState = 0;
        drawImage(ScreenStartBitmap, 0, 0, 160, 240);
        playSoundB(Snd_Start, SND_STARTLEN, SND_STARTFREQ, 1);
    }
}