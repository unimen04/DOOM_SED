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

player set_player(float x, float y){
	player j;
	j.pos = set_coords(x,y);
	j.health=100;
	j.angle=0;
	return j;
}

//inutil en 3D
posLCD coords2LCD(coords pos){
	posLCD LCDpos;
	double posX = round(pos.x*CELL_SIZE);
	double posY = round(pos.y*CELL_SIZE);
	LCDpos.x = (uint8_t)posX;
	LCDpos.y = (uint8_t)posY;
	return LCDpos;
}

