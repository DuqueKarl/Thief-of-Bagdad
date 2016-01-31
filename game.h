#ifndef _GAME_H_
#define	_GAME_H_

#include "myLib.h"

#define CHAR_NAME(c) (64*c)

/* 128 sprites:
 * 0=Player(40=DoubleTile)
 * 1-40=Enemies (41-80=DoubleTile)
 * 
 * 80-110 Coins, etc
 * 111-128 GUI: Lifes, etc.
 */
#define TI2 40
#define TLI 120
#define TCO 81  // 15 on screen are a lot :)
#define TCP 100
#define TCA 105 // SPRITE's OAM's

SPRITE enemies[80];
SPRITE player;
SPRITE coins[60];
SPRITE carpets[5];    // Max: 3 on screen

int totalEnemies;
int totalCoins;
int totalCarpets;

int lifes;
int myCoins;

int coinsTimer;

// CHEATS
int cheatInvulnerable;  // Cheat for invulnerability

// Player
int playerY;
int playerX;

int lastFacing;
int timerWalk;
int animationWalk;

int animationFlip;
int isJumping;
float jumpInc;
int canJump;
int timerJump;
int jumpY;
int frontflip;
float MAX_JUMP;
int canKill;

int collisions;

int checkPoint;


void initSprites();
void loadSprites();

void loopGame(int lifes, int myCoins);
void moveSprites();
void movePlayer();

void putLifes(int lifes);
void putMyCoins(int myCoins);

void moveHero(int p, int r, int c, int off1, int off2, int ATTR1);
void deathHero(int p, int r, int c);
void initPlayer();

void initEnemies();
void initCarpets();

//Pikachu enemies
void initEnemy(int row, int col, int col2, int t);
void moveEnemy(int e);
void moveScorpio(int e, int off1, int off2, int ATTR1);
void moveSoldier1(int e, int off1, int off2, int ATTR1);
void dyingEnemy(int e);

int checkCollisions();
int distY(int y1, int y2);
int distX(int x1, int x2);

    
void putCoins(int row, int col, int max);
void showCoins();
void showCP();
void showCarpets();

#endif	/* GAME_H */

