/*
 * Autores: Jorge Ameneiro y Unai Mendoza
 * Fecha:   08/Marzo/2025
 * 
 * Implementa todas las instrucciones necesarias para
 * dibujar diferentes figuras en una pantalla LCD
 */
 
#include <stdio.h>
#include <stdlib.h>
#include "Buffer.h"
#include "SPI_LCD.h"

//contiene los numeros del 0 al 9
static const uint8_t digitos[10][FONT_SIZE] ={
	{0xF, 0x9, 0x9, 0x9, 0xF}, //0
	{0x1, 0x1, 0x1, 0x1, 0x1}, //1
	{0xF, 0x1, 0xF, 0x8, 0xF}, //2
	{0xF, 0x1, 0xF, 0x1, 0xF}, //3
	{0x9, 0x9, 0xF, 0x1, 0x1}, //4
	{0xF, 0x8, 0xF, 0x1, 0xF}, //5
	{0xF, 0x1, 0xF, 0x9, 0xF}, //6
	{0xF, 0x1, 0x1, 0x1, 0x1}, //7
	{0xF, 0x9, 0xF, 0x9, 0xF}, //8
	{0xF, 0x9, 0xF, 0x1, 0x1}, //9
};

//contiene las letras del abecedario
static const uint8_t letras [NUM_LETRAS_ABECEDARIO+1][FONT_SIZE] ={
	{0x1F, 0x11, 0x1F, 0x11, 0x11}, // A
	{0x1E, 0x11, 0x1E, 0x11, 0x1E}, // B
	{0x1F, 0x10, 0x10, 0x10, 0x1F}, // C
	{0x1E, 0x11, 0x11, 0x11, 0x1E}, // D
	{0x1F, 0x10, 0x1E, 0x10, 0x1F}, // E
	{0x1F, 0x10, 0x1E, 0x10, 0x10}, // F
	{0x1E, 0x10, 0x13, 0x11, 0x1E}, // G
	{0x11, 0x11, 0x1F, 0x11, 0x11}, // H
	{0x1F, 0x04, 0x04, 0x04, 0x1F}, // I
	{0x01, 0x01, 0x01, 0x11, 0x0E}, // J
	{0x11, 0x12, 0x1C, 0x12, 0x11}, // K
	{0x10, 0x10, 0x10, 0x10, 0x1F}, // L
	{0x11, 0x1B, 0x15, 0x11, 0x11}, // M
	{0x11, 0x19, 0x15, 0x13, 0x11}, // N
	{0x0E, 0x11, 0x11, 0x11, 0x0E}, // O
	{0x1E, 0x11, 0x1E, 0x10, 0x10}, // P
	{0x1E, 0x11, 0x11, 0x13, 0x1D}, // Q
	{0x1E, 0x11, 0x1E, 0x12, 0x11}, // R
	{0x1E, 0x10, 0x1E, 0x01, 0x1E}, // S
	{0x1F, 0x04, 0x04, 0x04, 0x04}, // T
	{0x11, 0x11, 0x11, 0x11, 0x1F}, // U
	{0x11, 0x11, 0x11, 0x0A, 0x04}, // V
	{0x11, 0x11, 0x15, 0x15, 0x0A}, // W
	{0x11, 0x0A, 0x04, 0x0A, 0x11}, // X
	{0x11, 0x11, 0x1E, 0x01, 0x1E}, // Y
	{0x1F, 0x08, 0x04, 0x02, 0x1F}, // Z
	{0x00, 0x00, 0x00, 0x00, 0x00} // espacio, no funciona si se escribe en ascii, pero sirve usando posicion en el array
};

// Set all positions in buffer to zero
void Buffer_Reset(uint8_t* buffer) {
	for(int i=0;i<LCD_MEM_SIZE;i++)
		buffer[i]=0;
}

// Draw pixel in buffer at (x,y)
void Buffer_SetPixel(uint8_t* buffer, uint8_t x, uint8_t y) {
	//buscamos en que página está el pixel a dibujar
	uint8_t pagina= y/LCD_NUM_ROWS_PER_PAGE;
	//ubicamos el pixel dentro de la página
	uint8_t bit= y%LCD_NUM_ROWS_PER_PAGE;
	
	//modificamos el bit dentro de la memoria
	buffer[pagina*LCD_NUM_COLS+x] |= 1<<bit;
}

// Draw horizontal line in buffer from (xi,y) to (xf,y)
void Buffer_DrawLineH(uint8_t* buffer, uint8_t xi, uint8_t xf, uint8_t y) {
	for(int i=xi;i<xf;i++)
		Buffer_SetPixel(buffer,i,y);
}

// Draw vertical line in buffer from (x,yi) to (x,yf)
void Buffer_DrawLineV(uint8_t* buffer, uint8_t x, uint8_t yi, uint8_t yf) {
	for(int i=yi;i<yf;i++)
		Buffer_SetPixel(buffer,x,i);
}

//dibuja lineas horizontales, si se quiere dibujar vertical u horizontal usar Buffer_DrawLineH o
// Buffer_DrawLineH, son mas rapidas, utiliza bresenham para dibujar una linea
void Buffer_DrawLine(uint8_t* buffer, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2){
	uint8_t x=x1;
	uint8_t y=y1;
	
	uint8_t dx = x2-x1;				//diferencia en el eje horizontal
	uint8_t dy = abs(y2-y1);		//diferencia en el eje vertical
	
	int8_t sy = (y1<y2) ? 1 : -1;	//sentido en el eje y 
	int8_t sx = (x1<x2) ? 1 : -1;	//sentido en el eje x 
	
	int8_t E = 2*dy-dx;				//error inicial
	
	for(; x!=x2; x+=sx){
		Buffer_SetPixel(buffer,x,y);
		
		/* ajusta el error basado en la diferencia del eje horizontal y aumenta o disminuye la posicion en
		 * y dependiendo del sentido si el error es mayor o igual a 0, si no ajusta el error basado en la
		 * diferencia del eje vertical*/
		if(E>=0){
			E-=2*dx;
			y+=sy;
		}else
			E+=2*dy;
	}
}
// Draw rectangle in buffer at (x,y) with size (width,height)
void Buffer_DrawRect(uint8_t* buffer, uint8_t x, uint8_t y, uint8_t width, uint8_t height) {
	for(int i=y;i<y+height;i++)
			Buffer_DrawLineH(buffer,x,x+width,i);
}

//dibuja un rectangulo blanco
void Buffer_DrawBRect(uint8_t* buffer, uint8_t x, uint8_t y, uint8_t width, uint8_t height){
	Buffer_DrawLineH(buffer,x,x+width,y);
	Buffer_DrawLineH(buffer,x,x+width,y+height);
	Buffer_DrawLineV(buffer,x,y,y+height);
	Buffer_DrawLineV(buffer,x+width,y,y+height);
}

//escribe un digitito en las posciones x y
void Buffer_DrawDigit(uint8_t* buffer, uint8_t digito, uint8_t x, uint8_t y){
	for(int i=0;i<FONT_SIZE;i++)
		for(int j=0;j<FONT_SIZE;j++){
			if(digitos[digito][i] & (1 << (4-j)))
				Buffer_SetPixel(buffer,x+j,y+i);
		}
}

/* escribe un numero de uno o varios digitos empezando en x y, separados entre si por un offset
 * convierte a char el numero y lo recorre con un puntero, despues lo vuelve a cambiar a entero
 * y lo escribe llamando a la funcion Buffer_DrawDigit*/
void Buffer_DrawNum(uint8_t* buffer, int num, uint8_t x, uint8_t y){
	uint8_t offset=0;
	char numero[10];
	if(num<0){
		num=-num;
		Buffer_DrawLineH(buffer,x,x+FONT_SIZE,y+FONT_SIZE/2);
		offset+=OFFSET;
	}
	sprintf(numero,"%lu",num);
	for(char *p=numero;*p;p++){
		uint8_t digito = *p - '0'; //conversion maligna a entero
		Buffer_DrawDigit(buffer,digito,x+offset,y);
		offset+=OFFSET;
	}
}

//escribe una letra en las posciones x y
void Buffer_DrawLetter(uint8_t* buffer, char letter, uint8_t x, uint8_t y){
	uint8_t pos_letra = letter - 'A';
	for(int i=0; i < FONT_SIZE; i++)
		for(int j=0; j < FONT_SIZE; j++){
			if(letras[pos_letra][i] & (1 << (4-j)))
				Buffer_SetPixel(buffer, x+j, y+i);
		}

}

/* escribe un texto empezando en x y, separados entre si por un offset
 * recorre la cadena con un puntero llamando a la funcion Buffer_DrawLetter*/
void Buffer_DrawWord(uint8_t* buffer, char *texto, uint8_t x, uint8_t y){
	uint8_t offset=0;
	for(char* p=texto; *p; p++){
		Buffer_DrawLetter(buffer, *p, x+offset, y);
		offset+=OFFSET;
	}
}
