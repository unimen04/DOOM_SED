#include "structure.h"

/*
El mapa está formado por celdas de 4x4 pixeles, donde 1 representa una pared y 0 un espacio vacío.
El mapa tiene un tamaño de 32x8 celdas, lo que equivale a 128x32 pixeles. (definido en structure.h)
*/
uint8_t map[MAP_HEIGHT][MAP_WIDTH]={
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1},
	{1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1},
	{1,0,1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

//x e y son las coordenadas del jugador en el mapa, no en la pantalla.
coords set_coords(float x, float y){
	coords coords = {x,y};
	return coords;
}

//sabiendo la posicion del jugador en el mapa, devuelve la celda correspondiente.
cell obtainCell(coords p1){
	cell cellPos;
	cellPos.x = (uint8_t)p1.x;
	cellPos.y = (uint8_t)p1.y;
	return cellPos;
}

//x e y dan las coordenadas del jugador en el mapa, no en la pantalla. 
player set_player(float x, float y){
	player j;
	j.pos = set_coords(x,y);
	j.health=100;
	j.angle=0;
	return j;
}

//INUTIL EN 3D
//convierte las coordenadas del jugador en el mapa a coordenadas de la pantalla LCD
//la función round devuelve un tipo double, por lo que se redondea a un entero uint8_t
posLCD coords2LCD(coords pos){
	posLCD LCDpos;
	double posX = round(coords.x*CELL_SIZE);
	double posY = round(coords.y*CELL_SIZE);
	LCDpos.x = (uint8_t)posX;
	LCDpos.y = (uint8_t)posY;
	return LCDpos;
}


