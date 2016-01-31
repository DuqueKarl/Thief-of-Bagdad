#include "myLib.h"

unsigned short *videoBuffer = (u16 *)0x6000000;
unsigned short *frontBuffer = (u16 *)0x6000000;
unsigned short *backBuffer =  (u16 *)0x600A000;

DMA *dma = (DMA *)0x40000B0;


void setPixel3(int row, int col, unsigned short color) {
    videoBuffer[OFFSET(row, col, SCREENWIDTH)] = color;
}

void drawRect3(int row, int col, int height, int width, unsigned short color) {
    unsigned short c = color;

    int i;
    for(i = 0; i < height; i++) {
        DMANow(3, &c, &videoBuffer[OFFSET(row+i, col, SCREENWIDTH)], (width) | DMA_SOURCE_FIXED);
    }
}

void drawImage3(const unsigned short* image, int row, int col, int height, int width) {
    int i;
    for(i = 0; i < height; i++) {
        DMANow(3, &image[OFFSET(i,0, width)], &videoBuffer[OFFSET(row+i, col, SCREENWIDTH)], (width));
    }
}

void fillScreen3(unsigned short color) {
    unsigned short c = color;

    DMANow(3, &c, videoBuffer, (240*160) | DMA_SOURCE_FIXED);	
}

void setPixel4(int row, int col, unsigned char colorIndex) {
    unsigned short pixels = videoBuffer[OFFSET(row, col/2, SCREENWIDTH/2)];

    if(col % 2 == 0) // even
    {
        pixels &= 0xFF << 8;
        videoBuffer[OFFSET(row, col/2, SCREENWIDTH/2)] = pixels | colorIndex;
    }
    else // odd
    {
        pixels &= 0xFF;
        videoBuffer[OFFSET(row, col/2, SCREENWIDTH/2)] = pixels | colorIndex << 8;
    }
}

void drawRect4(int row, int col, int height, int width, unsigned char colorIndex) {
    unsigned short pixels = colorIndex << 8 | colorIndex;

    int r;
    for(r = 0; r < height; r++) {
        if(col % 2 == 0) // even starting col
        {
            DMANow(3, &pixels, &videoBuffer[OFFSET(row + r, col/2, SCREENWIDTH/2)], (width/2) | DMA_SOURCE_FIXED);	
            if(width % 2 == 1) // if width is odd
            {
                setPixel4(row+r, col+width - 1, colorIndex);
            }
        }
        else // old starting col
        {
            setPixel4(row+r, col, colorIndex);

            if(width % 2 == 1) // if width is odd
            {
                DMANow(3, &pixels, &videoBuffer[OFFSET(row + r, (col+1)/2, SCREENWIDTH/2)], (width/2) | DMA_SOURCE_FIXED);
            }
            else  // width is even
            {
                DMANow(3, &pixels, &videoBuffer[OFFSET(row + r, (col+1)/2, SCREENWIDTH/2)], ((width/2)-1) | DMA_SOURCE_FIXED);
                setPixel4(row+r, col+width - 1, colorIndex);
            }
        }
    }	
}

void fillScreen4(unsigned char colorIndex) {
    unsigned short pixels = colorIndex << 8 | colorIndex;
    DMANow(3, &pixels, videoBuffer, ((240 * 160)/2) | DMA_SOURCE_FIXED);
}

void drawBackgoundImage4(const unsigned short* image) {
    DMANow(3, image, videoBuffer, ((240*160)/2) | DMA_SOURCE_INCREMENT | DMA_ON);
}

void drawImage4(const unsigned short* image, int row, int col, int height, int width) {
    int r;

    if (col%2)
        col++;

    for(r = 0; r < height; r++)
    {
        DMANow(3, &image[OFFSET(r,0,width/2)], &videoBuffer[OFFSET(row + r, col/2, SCREENWIDTH/2)], (width/2) | DMA_SOURCE_INCREMENT);	
    }

    // however, this is function is tricky because of 8-bit color
    // There are two ways to get over this issue
    // 1. make it so no matter what we'll start drawing the image on an even col
    //    also your images will have to have an even number of width i.e. 10 pixels wide

}


void drawImage(const unsigned short* image, int row, int col, int height, int width)
{
    // COMPLETE THIS FUNCTION USING DMA!!
    int i;
    for(i = 0; i < height; i++)
    {
        DMANow(3,&image[OFFSET(row+i,col,240)],&videoBuffer[OFFSET(row+i, col, 240)],width);
    }
}


void drawSubImage4(const unsigned short* sourceImage, int sourceRow, int sourceCol, int sourceWidth, 
				   int row, int col, int height, int width)
{
    int r;

    if (col%2)
        col++;

    for(r = 0; r < height; r++)
    {
        DMANow(3, &sourceImage[OFFSET(sourceRow*width + r, sourceCol, sourceWidth/2)], &videoBuffer[OFFSET(row + r, col/2, SCREENWIDTH/2)], (width/2) | DMA_SOURCE_INCREMENT);	
    }

    // COMPLETE THIS FUNCTION !!!!
    // This function will be used to display a section of an image 
    // This function will be similar to drawImage4 except you will be doing a little more math
    //   in regards to the image source

}

void loadPalette(const unsigned short* palette) {
    int i;
    for (i = 0; i < 255; i++) {
        PALETTE[i] = palette[i];
    }
}


void DMANow(int channel, void* source, void* destination, unsigned int control) {
    dma[channel].src = source;
    dma[channel].dst = destination;
    dma[channel].cnt = DMA_ON | control;
}

void waitForVblank() {
    while(SCANLINECOUNTER > 160);
    while(SCANLINECOUNTER < 160);
}


void flipPage() {
    if(REG_DISPCTL & BACKBUFFER) {
        REG_DISPCTL &= ~BACKBUFFER;
        videoBuffer = backBuffer;
    }
    else
    {
        REG_DISPCTL |= BACKBUFFER;
        videoBuffer = frontBuffer;
    }
}

