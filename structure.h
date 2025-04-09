#ifndef structure_h
#define structure_h

#include <LPC17xx.h>
#include <PIN_LPC17xx.h>
#include <GPIO_LPC17xx.h>
#include <math.h>
#include "SPI_LCD.h"

//constantes usadas para el mapa
#define CELL_SIZE 4
#define MAP_WIDTH (LCD_NUM_COLS/CELL_SIZE)
#define MAP_HEIGHT (LCD_NUM_ROWS/CELL_SIZE)

#define WALL 1
#define EMPTY 0
#define ENEMY 2
#define PLAYER 3

extern uint8_t map[MAP_HEIGHT][MAP_WIDTH];

//eliminar al pasar a 3D
typedef struct{
	uint8_t x;
	uint8_t y;
}posLCD;

typedef struct{
	float x;
	float y;
}coords;

typedef struct{
	uint8_t x;
	uint8_t y; 
}cell;

typedef struct{
	coords pos;
	float distance;
	uint8_t colission;
}ray;

coords set_coords(float x, float y);

cell obtainCell(coords p1);

float distance(coords p1, coords p2);

uint8_t colissionCell(cell checkCell);

uint8_t colission(coords pos);

posLCD coords2LCD(coords pos);
#endif
