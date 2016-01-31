#include "myLib.h"
#include "sound.h"



int startSounds() {
	setupInterrupts();
	setupSounds();
	
        //playSoundA(powerupsfx, POWERUPSFXLEN, POWERUPSFXFREQ, 0);
        // don't play sound B inside while loop !!!!! 
	//playSoundB(specialbgm2, SPECIALBGM2LEN, SPECIALBGM2FREQ, 1);
        //playSoundA(powerupsfx, POWERUPSFXLEN, POWERUPSFXFREQ, 0);
}

void setupSounds() {
	// This function will enable sounds
	REG_SOUNDCNT_X = SND_ENABLED;

	REG_SOUNDCNT_H = SND_OUTPUT_RATIO_100 |
                
                         DSA_OUTPUT_RATIO_100 | 
                         DSA_OUTPUT_TO_BOTH |
                         DSA_TIMER0 | 
                         DSA_FIFO_RESET |
                
                         DSB_OUTPUT_RATIO_100 |
                         DSB_OUTPUT_TO_BOTH | 
                         DSB_TIMER1 | 
                         DSB_FIFO_RESET;

	REG_SOUNDCNT_L = 0;
}

void playSoundA( const unsigned char* sound, int length, int frequency, int isLooping ) {

	// resets the dma and vbcount!
        dma[1].cnt = 0;
        vbCountA = 0; 
                
	// This function will play a sound out of sound channel A        
	// fist compute the timer interval for the sound.  The number of cycles divided by the frequency of the sound
        int interval = 16777216/frequency; 
        
	// then setup the DMA channel for this sound
        DMANow(1, sound, REG_FIFO_A, DMA_DESTINATION_FIXED | 
                                        DMA_AT_REFRESH | 
                                        DMA_REPEAT | 
                                        DMA_32);
        
	// then setup the timer for this sound
        REG_TM0CNT = 0; 
	// then start the timer
        REG_TM0D = -interval; 
        REG_TM0CNT = TIMER_ON; 

	// you may have to create additional global variables for these functions to work (to stop the sounds later)
        soundA.data = sound; 
        soundA.length = length; 
        soundA.frequency = frequency; 
        soundA.isPlaying = 1; 
        soundA.loops = isLooping; 
        soundA.duration = ((60*length)/frequency) - ((length/frequency)*3) -1; 
}


void playSoundB( const unsigned char* sound, int length, int frequency, int isLooping ) {
	// This function will play a sound out of sound channel B
	// refer to playSoundA instructions
	// resets the dma and vbcount!
        dma[2].cnt = 0;
        vbCountB = 0; 
                
	// This function will play a sound out of sound channel A
        
	// fist compute the timer interval for the sound.  The number of cycles divided by the frequency of the sound
        int interval = 16777216/frequency; 
        
	// then setup the DMA channel for this sound
        DMANow(2, sound, REG_FIFO_B, DMA_DESTINATION_FIXED | 
                                        DMA_AT_REFRESH | 
                                        DMA_REPEAT | 
                                        DMA_32);
        
	// then setup the timer for this sound
        REG_TM1CNT = 0; 
	// then start the timer
        REG_TM1D = -interval; 
        REG_TM1CNT = TIMER_ON; 

	// you may have to create additional global variables for these functions to work (to stop the sounds later)
        soundB.data = sound; 
        soundB.length = length; 
        soundB.frequency = frequency; 
        soundB.isPlaying = 1; 
        soundB.loops = isLooping; 
        soundB.duration = ((60*length)/frequency)-((length/frequency)*3)-1; 
}

void setupInterrupts()
{
	REG_IME = 0;
	REG_INTERRUPT = (unsigned int)interruptHandler;
	// accept interrupts
	REG_IE |= INT_VBLANK;
	// turn on interrupt sending
	REG_DISPSTAT |= INT_VBLANK_ENABLE;

	REG_IME = 1;
}

void interruptHandler()
{
	REG_IME = 0;

	if(REG_IF & INT_VBLANK) {
		// MODIFY THIS FUNCTION !!!
		// Add code to stop/repeat sounds here
                vbCountA++; 
                if(vbCountA>=soundA.duration){
                    REG_TM0CNT = 0; 
                    dma[1].cnt = 0; 
                    
                    if(soundA.loops) {
                        playSoundA(soundA.data, soundA.length, soundA.frequency, soundA.loops);
                    } else {
                        soundA.isPlaying = false;
                    }
                }
                
                vbCountB++; 
                if(vbCountB>=soundB.duration){
                    REG_TM1CNT = 0; 
                    dma[2].cnt = 0; 
                    
                    if(soundB.loops && soundB.isPlaying) {
                        playSoundB(soundB.data, soundB.length, soundB.frequency, soundB.loops);
                    } else {
                        soundB.isPlaying = false;
                    }
                }
                
		REG_IF = INT_VBLANK; 
	}
        // (â•¯Â°â–¡Â°ï¼‰â•¯ï¸µ â”»â”â”» 
        // â”¬â”€â”€â”¬ï»¿ ãƒŽ( ã‚œ-ã‚œãƒŽ)
	REG_IME = 1;
}



void stopSoundB() {
    dma[2].cnt = 0;
    REG_TM1CNT = 0; 
    soundB.isPlaying = false;
}