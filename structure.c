#include "structure.h"

uint8_t map[MAP_HEIGHT][MAP_WIDTH]={
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1},
	{1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

coords set_coords(float x, float y){
	coords coords = {x,y};
	return coords;
}

cell obtainCell(coords p1){
	cell cellPos;
	cellPos.x = (uint8_t)p1.x;
	cellPos.y = (uint8_t)p1.y;
	return cellPos;
}

//calcula la distancia entre dos puntos
float distance(coords p1, coords p2){
	return sqrt(pow(p1.x-p2.x,2)+pow(p1.y-p2.y,2));
}

//comprueba si la celda esta ocupada
uint8_t colissionCell(cell checkCell){
	if (checkCell.x >= MAP_WIDTH || checkCell.y >= MAP_HEIGHT)
        return 1; // Fuera del mapa = colisi�n
	
	return map[checkCell.y][checkCell.x];
}

//colision del jugador
uint8_t colission(coords pos){
	cell cellCheck = obtainCell(pos);
	return colissionCell(cellCheck);
}


