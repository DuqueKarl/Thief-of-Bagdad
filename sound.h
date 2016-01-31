#ifndef SOUND_H
#define	SOUND_H

typedef struct{
    unsigned char* data; 
    int length; 
    int frequency;
    int loops; 
    int isPlaying; 
    int duration;     
}SOUND;

SOUND soundA; 
SOUND soundB; 
int vbCountA; 
int vbCountB; 

void initialize();
void update();
void draw();

void setupSounds();
void playSoundA( const unsigned char* sound, int length, int frequency, int isLooping );
void playSoundB( const unsigned char* sound, int length, int frequency, int isLooping );

void stopSoundB();

void setupInterrupts();
void interruptHandler();


#endif	/* SOUND_H */

