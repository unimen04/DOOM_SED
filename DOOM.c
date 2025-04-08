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
#define RES_DIVIDER 2

#define PARED 1
#define ENEMIGO 2

uint8_t level= 0;
player j1;

//comprueba si la celda esta ocupada
uint8_t colissionCell(cell checkCell){
	if (checkCell.x >= MAP_WIDTH || checkCell.y >= MAP_HEIGHT)
        return 1; // Fuera del mapa = colisión
	
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
	ray rayEnd;
	
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
			rayLength.x+=unitStepSize.x;
			distance = rayLength.x;
		}else{
			mapCheck.y+=stepy;
			rayLength.y+=unitStepSize.y;
			distance = rayLength.y;
		}
		//comprobar si se ha encontrado la casilla
		tileFound =colissionCell(mapCheck);
	}
	if(distance < 1) distance = 1;
	rayEnd.pos=set_coords(0,0);
	rayEnd.distance = distance * cos(angle-j1.angle);
	rayEnd.colission = tileFound;
	if(tileFound){
		rayEnd.pos.x = rayStart.x + rayDir.x*distance;
		rayEnd.pos.y = rayStart.y + rayDir.y*distance;
	}
	//__breakpoint(255);
	return rayEnd;
}

void draw3D(){
	const float d = (LCD_NUM_ROWS/2)/tan(FOV2RAD);
	//reset del buffer
	Buffer_Reset(buffer);
	
	//casteo de rayos
	for (uint8_t i = 0; i < NUM_RAYS; i++){
    // Calcula el ángulo para este rayo
    float camera_x = 2 * (double)i / NUM_RAYS - 1;
    float ray_angle = j1.angle + atan(camera_x * tan((FOV/2.0)*DEG2RAD));
    
    ray actualRay = castRay3D(ray_angle);
    
    // Calcula la altura de la pared proyectada
    uint8_t wallHeight = (uint8_t)((CELL_SIZE * d) / actualRay.distance); if(wallHeight > LCD_NUM_ROWS) wallHeight = LCD_NUM_ROWS; 
    
    uint8_t centerY = LCD_NUM_ROWS / 2;
    uint8_t startY = centerY - wallHeight / 2;
    uint8_t endY   = centerY + wallHeight / 2;
    
    // Calcula la posición horizontal en la pantalla
    uint8_t screenX = i * RES_DIVIDER;
    
    // Dibuja la columna de la pared
    Buffer_DrawLineV(buffer, screenX, startY, endY);
    
    // Opcional: Rellenar columnas intermedias para evitar huecos
   for (uint8_t j = 1; j < RES_DIVIDER; j++){
        Buffer_DrawLineV(buffer, screenX+j, startY, endY);
    }
	 __breakpoint(255);
	}
}

void move_player(){
	coords newPos = j1.pos;
	coords pDir = set_coords(cos(j1.angle),sin(j1.angle));
	coords pDir90 = set_coords(-pDir.y,pDir.x); // = set_coords(cos(j1.angle+PIHALPH),sin(j1.angle+PIHALPH))
	
	if(up_pressed){
		up_pressed=0;
		newPos.x+=PLAYER_SPEED*pDir.x;
		newPos.y+=PLAYER_SPEED*pDir.y;
	}else if(down_pressed){
		down_pressed=0;
		newPos.x-=PLAYER_SPEED*pDir.x;
		newPos.y-=PLAYER_SPEED*pDir.y;
	}else if(right_pressed){
		right_pressed=0;
		newPos.x+=PLAYER_SPEED*pDir90.x;
		newPos.y+=PLAYER_SPEED*pDir90.y;
	}else if(left_pressed){
		left_pressed=0;
		newPos.x-=PLAYER_SPEED*pDir90.x;
		newPos.y-=PLAYER_SPEED*pDir90.y;
	}
	
	//solo se actualiza la posicion si no ha habido colision
	if(!colission(newPos))
		j1.pos=newPos;
}

void init_game(void){
	j1=set_player(29,2);
}

void loop(void){
	read_joy();
	
	if(interrupt){
		interrupt=0;
		
		//ADC, ajustar el valor entre 1 y 2pi
		if(read_pot())
			j1.angle=(pot1_read*PI2)/MAX_POT_VALUE;
		
		//mover la posicion del jugador
		move_player();
		
		draw3D();
		
		SPILCD_Transfer(spiDrv1, buffer);
	}
}
