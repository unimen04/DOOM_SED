#include <math.h>
#include <stdlib.h>
#include "structure.h"
#include "LPC1768_func.h"
#include "DOOM.h"
#include "Buffer.h"

#define DIST 7

#define PI 3.14159265359
#define PI2 (2*PI)
#define DEG2RAD (PI/180)

#define FOV 60 //angulo en grados del campo de vision del jugador
#define NUM_RAYS (FOV) //un angulo por grado
#define FOV2 (FOV/2)
#define FOV2RAD (FOV2*DEG2RAD)
#define RES (LCD_NUM_COLS/NUM_RAYS) //divisor de resolucion
#define PROYECTION_HEIGHT (LCD_NUM_ROWS*D)
#define CENTER_Y (LCD_NUM_ROWS/2) 

#define PARED 1
#define ENEMIGO 2

uint8_t level= 0;
player j1;

//comprueba si la celda esta ocupada
uint8_t colissionCell(cell checkCell){
	return map[checkCell.y][checkCell.x];
}

//colision del jugador
uint8_t colission(coords pos){
	cell cellCheck = obtainCell(pos);
	return colissionCell(cellCheck);
}

//dibuja un rayo en base a la distancia hasta la posicion en el que el rayo colisiona con una pared usando DDA
ray castRay3D(float angle){
	coords rayStart = j1.pos; //coordenada inicial del rayo
	coords rayDir = set_coords(cos(angle),sin(angle)); //direccion del rayo
	coords unitStepSize = set_coords(sqrt(1+(rayDir.y/rayDir.x)*(rayDir.y/rayDir.x)),sqrt(1+(rayDir.x/rayDir.y)*(rayDir.x/rayDir.y))); //vector unitario del rayo
	
	//verifica las celdas del mapa, se inicializa en la celda inicial en la que se encuentra el jugador
	cell mapCheck = obtainCell(rayStart);
	coords rayLength;
	int8_t stepx;
	int8_t stepy;
	
	if(rayDir.x <0){
		stepx=-1;
		rayLength.x = (rayStart.x - (float)mapCheck.x)*unitStepSize.x;
	}else{
		stepx=1;
		rayLength.x = ((float)mapCheck.x-rayStart.x)*unitStepSize.x;
	}
	
	if(rayDir.y <0){
		stepy=-1;
		rayLength.y = (rayStart.y - (float)mapCheck.y)*unitStepSize.y;
	}else{
		stepy=1;
		rayLength.y = ((float)(mapCheck.y+1)-rayStart.y)*unitStepSize.y;
	}
	
	uint8_t tileFound = 0;
	float distance=0;
	while(!tileFound && distance < LCD_NUM_COLS){
		//avanzar el rayo
		if(rayLength.x < rayLength.y){
			mapCheck.x+=stepx;
			distance = rayLength.x;
			rayLength.x+=unitStepSize.x;
		}else{
			mapCheck.y+=stepy;
			distance = rayLength.y;
			rayLength.y+=unitStepSize.y;
		}
		//comprobar si se ha encontrado la casilla
		tileFound = colissionCell(mapCheck);
	}
	ray ray1;
	ray1.pos=set_coords(0,0);
	ray1.distance = distance;
	ray1.colision = tileFound;
	if(tileFound){
		ray1.pos.x = rayStart.x + rayDir.x*distance;
		ray1.pos.y = rayStart.y + rayDir.y*distance;
	}
	return ray1;
}

//----------------------------------------------------------2D------------------------------------------------------
//estas funciones dejan de ser utiles en 3D
void draw_map2D(){
	for(uint8_t i=0;i<MAP_HEIGHT;i++){
		uint8_t y=i*CELL_SIZE;
		for(uint8_t j=0;j<MAP_WIDTH;j++)
			if(map[i][j]==1){
				uint8_t x= j*CELL_SIZE;
				Buffer_DrawRect(buffer,x,y,CELL_SIZE,CELL_SIZE);
		}
	}
}

void draw_player2D(){
	posLCD playerLCD = coords2LCD(j1.pos);
	Buffer_SetPixel(buffer,posLCD.x,posLCD.y);
	
	//un rayo
	ray actualRay = castRay3D(0);
	posLCD rayLCD = coords2LCD(actualRay.pos);
	Buffer_DrawLine(buffer,playerLCD.x,playerLCD.y,rayLCD.x,rayLCD.y);
	
	//mazo rayos
	/*
	for(uint8_t i = 0; i<NUM_RAYS; i++){
		uint8_t degOffset = -FOV2 + i;
		float radianOffset = offset * DEG2RAD;
		ray actualRay = castRay3D(radianOffset);
		
		posLCD rayLCD = coords2LCD(actualRay.pos);
		Buffer_DrawLine(buffer,playerLCD.x,playerLCD.y,rayLCD.x,rayLCD.y);
	}*/
}
--------------------------------------------------------------------------------------------------------------------


void draw3D(){
	
	//probar 2D primero
	for(uint8_t i = 0; i<NUM_RAYS; i++){
		uint8_t degOffset = -FOV2 + i;
		float radianOffset = offset * DEG2RAD;
		ray actualRay = castRay3D(radianOffset);
		
		
		switch(actualRay.colision){
			case PARED:
				//dibuja una pared
				uint8_t wallHeight = (LCD_NUM_ROWS * d)/actualRay.distance;
				if(wallHeight > LCD_NUM_ROWS) wallHeight=LCD_NUM_ROWS;
				uint8_t startY = CENTER_Y - wallHeight/2;
				uint8_t endY = CENTER_Y + wallHeight/2;
				if(endY > LCD_NUM_ROWS) endY = LCD_NUM_ROWS;
				uint8_t screenX = (i/NUM_RAYS)*LCD_NUM_COLS;
				Buffer_DrawLineV(buffer,screenX,startY,endY);
			break;
			
			csae ENEMIGO:
				//codigo para renderizar a un enemigo
			break;
				
		}
		
	}
}

void move_player(){
	coords newPos = j1.pos;
	
	if(up_pressed){
		up_pressed=0;
		newPos.y-=PLAYER_SPEED;
	}else if(down_pressed){
		down_pressed=0;
		newPos.y+=PLAYER_SPEED;
	}else if(right_pressed){
		right_pressed=0;
		newPos.x+=PLAYER_SPEED;
	}else if(left_pressed){
		left_pressed=0;
		newPos.x-=PLAYER_SPEED;
	}
	
	//solo se actualiza la posicion si no ha habido colision
	if(!colission(newPos))
		j1.pos=newPos;
}

void init_game(void){
	j1=set_player(MAP_WIDTH/2,MAP_HEIGHT/2);
}

void loop(void){
	read_joy();
	
	if(interrupt){
		interrupt=0;
		
		//ADC, ajustar el valor entre 1 y 2pi
		read_pot();
		j1.angle=(pot1_read*PI2)/MAX_POT_VALUE;
		
		//mover la posicion del jugador
		move_player();
		
		Buffer_Reset(buffer);
		draw_map2D();
		draw3D();
		
		SPILCD_Transfer(spiDrv1, buffer);
	}
}