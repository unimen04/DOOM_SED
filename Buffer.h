#ifndef BUFFER_H_
#define BUFFER_H_

#define FONT_SIZE 5
#define OFFSET 6
#define NUM_LETRAS_ABECEDARIO 27

#include <stdint.h>

// Set all positions in buffer to zero
void Buffer_Reset(uint8_t* buffer);

// Draw pixel in buffer at (x,y)
void Buffer_SetPixel(uint8_t* buffer, uint8_t x, uint8_t y);

// Draw horizontal line in buffer from (xi,y) to (xf,y)
void Buffer_DrawLineH(uint8_t* buffer, uint8_t xi, uint8_t xf, uint8_t y);

// Draw vertical line in buffer from (x,yi) to (x,yf)
void Buffer_DrawLineV(uint8_t* buffer, uint8_t x, uint8_t yi, uint8_t yf);

void Buffer_DrawLine(uint8_t* buffer, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);

// Draw rectangle in buffer at (x,y) with size (width,height)
void Buffer_DrawRect(uint8_t* buffer, uint8_t x, uint8_t y, uint8_t width, uint8_t height);

// Draw filled rectangle in buffer at (x,y) with size (width,height)
void Buffer_DrawBRect(uint8_t* buffer, uint8_t x, uint8_t y, uint8_t width, uint8_t height);

// Draw digit in buffer at (x,y)
void Buffer_DrawDigit(uint8_t* buffer, uint8_t digito, uint8_t x, uint8_t y);

// Draw number in buffer at (x,y)
void Buffer_DrawNum(uint8_t* buffer, int num, uint8_t x, uint8_t y);

// Draw letter in buffer at (x,y)
void Buffer_DrawLetter(uint8_t* buffer, char letter, uint8_t x, uint8_t y);

// Draw string in buffer at (x,y)
void Buffer_DrawWord(uint8_t* buffer, char *texto, uint8_t x, uint8_t y);

#endif /* BUFFER_H_ */
