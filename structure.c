#include "structure.h"

/*
MAPA
1 -> pared
2 -> enemigo
3 -> spawn del jugador
4 -> botiquin
*/
uint8_t map[MAP_HEIGHT][MAP_WIDTH]={
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,3,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,2,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1},
	{1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};


/*
uint8_t map[MAP_HEIGHT][MAP_WIDTH] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,1,0,0,0,0,1,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1},
    {1,0,1,1,0,1,0,1,1,0,1,0,1,0,1,0,1,1,0,1,0,1,1,0,1,1,0,0,1,0,0,1},
    {1,0,1,0,0,0,0,1,0,0,1,0,1,0,0,0,0,1,0,1,0,0,1,0,0,1,0,0,1,0,0,1},
    {1,0,1,0,1,1,0,1,0,1,1,0,1,0,1,1,0,1,0,1,1,0,1,0,1,1,0,0,1,0,0,1},
    {1,0,0,0,1,3,0,0,0,0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,0,2,0,0,1,0,0,1},
    {1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,0,1,1,0,1,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,1,0,0,1},
    {1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,0,1},
    {1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,1},
    {1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1},
    {1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,0,1},
    {1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,2,0,1,0,0,1},
    {1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,1,0,0,1},
    {1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,0,1},
    {1,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,4,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};*/

//meter coordenadas en una estructura, dadas x e y
coords set_coords(float x, float y){
	coords coords = {x,y};
	return coords;
}

//dadas unas coordenadas, obtener la posición en la celda
cell obtainCell(coords p1){
	cell cellPos;
	cellPos.x = (uint8_t)p1.x;
	cellPos.y = (uint8_t)p1.y;
	return cellPos;
}

coords obtainCoords(cell cellPos){
	coords p1;
	p1.x = (float)cellPos.x;
	p1.y = (float)cellPos.y;
	return p1;
}

//devuelve el minimo entre a y b
uint8_t min(uint8_t a, uint8_t b){
	return (a < b) ? a : b;
}

//devuelve el maximo entre a y b
uint8_t max(uint8_t a, uint8_t b){
	return (a > b) ? a : b;
}

//calcula la distancia entre dos puntos
float distance2(coords p1, coords p2){
	return sqrt(pow(p1.x-p2.x,2)+pow(p1.y-p2.y,2));
}

//comprueba si la celda esta ocupada
uint8_t colissionCell(cell cellCheck){
	if (cellCheck.x >= MAP_WIDTH || cellCheck.y >= MAP_HEIGHT)
        return WALL; // Fuera del mapa = colision
	
	return map[cellCheck.y][cellCheck.x];
}

//colision del jugador con el mapa
uint8_t colission(coords pos){
	cell cellCheck = obtainCell(pos);
	return colissionCell(cellCheck);
}


//INUTIL EN 3D
//convierte las coordenadas del jugador en el mapa a coordenadas de la pantalla LCD
//la función round devuelve un tipo double, por lo que se redondea a un entero uint8_t
posLCD coords2LCD(coords pos){
	posLCD LCDpos;
	double posX = round((double)pos.x*CELL_SIZE);
	double posY = round((double)pos.y*CELL_SIZE);
	LCDpos.x = (uint8_t)posX;
	LCDpos.y = (uint8_t)posY;
	return LCDpos;
}

