#ifndef structure_h
#define structure_h

#include <stdlib.h>
#include <LPC17xx.h>
#include <PIN_LPC17xx.h>
#include <GPIO_LPC17xx.h>
#include <math.h>
#include "SPI_LCD.h"

//constantes de pi
#define PI 3.14159265359
#define PI2 (2*PI)
#define DEG2RAD (PI/180)

//constantes usadas para el mapa
#define CELL_SIZE 4
#define MAP_WIDTH (LCD_NUM_COLS/CELL_SIZE)
#define MAP_HEIGHT (LCD_NUM_ROWS/CELL_SIZE)
/*#define MAP_HEIGHT 20
#define MAP_WIDTH 32*/

#define WALL 1
#define EMPTY 0
#define ENEMY 2
#define PLAYER 3
#define HEALTHPACK 4
#define FIREBALL 5

extern uint8_t map[MAP_HEIGHT][MAP_WIDTH];

//eliminar al pasar a 3D
typedef struct{
	uint8_t x,y;
}posLCD;

typedef struct{
	float x,y;
}coords;

typedef struct{
	uint8_t x,y; 
}cell;

typedef struct{
	cell endCell;		// celda de colision
	float distance;		// distancia al objeto colisionado
	uint8_t colission; // tipo de colision (pared, enemigo, etc.)
	uint8_t entityUID; // UID de la entidad colisionada (si corresponde)
}ray;

coords set_coords(float x, float y);

cell obtainCell(coords p1);

coords obtainCoords(cell cellPos);

uint8_t min(uint8_t a, uint8_t b);
uint8_t max(uint8_t a, uint8_t b);

float distance2(coords p1, coords p2);

uint8_t colissionCell(cell cellCheck);

uint8_t colission(coords pos);

posLCD coords2LCD(coords pos);
#endif
