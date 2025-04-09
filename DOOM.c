#include <math.h>
#include <stdlib.h>
#include "structure.h"
#include "LPC1768_func.h"
#include "DOOM.h"
#include "Buffer.h"
#include "entity.h"

#define DIST 3

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

float d; //distancia entre el jugador y la pantalla, se calcula en base a la altura de la pantalla y el angulo de vision

//inicializa el juego, se inicializan las variables y se asignan los valores iniciales a las entidades
void init_game(void){
	//calculo de la distancia entre el jugador y la pantalla
	d = (LCD_NUM_ROWS/2)/tan(FOV2RAD);
	//inicializa la semilla de generacion de numeros aleatorios
	srand(time(NULL));

	uint8_t actualEntity=0;
	for(uint8_t i=0;i<MAP_HEIGHT;i++)
		for(uint8_t j=0;j<MAP_WIDTH;j++)
			switch(map[i][j]){
				case PLAYER:
					j1=set_player(j,i);
					map[i][j]=EMPTY;
				break;
				case ENEMY:
					entities[actualEntity]=set_entity(j,i,ENTITY_TYPE_ENEMY);
					actualEntity++;
				break;
			}

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

void draw_wall(uint8_t rayNum, ray actualRay){
	// Calcula la altura de la pared proyectada
	uint8_t wallHeight = (uint8_t)((CELL_SIZE * d) / actualRay.distance); if(wallHeight > LCD_NUM_ROWS) wallHeight = LCD_NUM_ROWS; 
					
	uint8_t centerY = LCD_NUM_ROWS / 2;
	uint8_t startY = centerY - wallHeight / 2;
	uint8_t endY   = centerY + wallHeight / 2;

	// Calcula la posici�n horizontal en la pantalla
	uint8_t screenX = rayNum * RES_DIVIDER;

	// Dibuja la columna de la pared
	Buffer_DrawLineV(buffer, screenX, startY, endY);

	// Opcional: Rellenar columnas intermedias para evitar huecos
	for (uint8_t j = 1; j < RES_DIVIDER; j++)
		Buffer_DrawLineV(buffer, screenX+j, startY, endY);
}

void draw3D(){
	//reset del buffer
	Buffer_Reset(buffer);
	
	//casteo de rayos
	for (uint8_t i = 0; i < NUM_RAYS; i++){
    // Calcula el �ngulo para este rayo

		float camera_x = 2 * (double)i / NUM_RAYS - 1;
		float ray_angle = j1.angle + atan(camera_x * tan((FOV/2.0)*DEG2RAD));
		
		ray actualRay = castRay3D(ray_angle);
		
		switch(actualRay.colission){
			case PARED: {
				draw_wall(i,actualRay);
			break;
			}
			case ENEMIGO:
			break;
		}
	}
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
coords hallar_puntoDireccion(coords p1,float angle,uint8_t dist){
	coords p2;
	p2.x = p1.x + dist*cos(angle);
	p2.y = p1.y + dist*sin(angle);
	return p2;
}
void draw_player2D(){
	posLCD playerLCD = coords2LCD(j1.pos);
	posLCD directionLCD = coords2LCD(hallar_puntoDireccion(j1.pos,j1.angle,DIST));
	Buffer_SetPixel(buffer,playerLCD.x,playerLCD.y);
	Buffer_SetPixel(buffer,directionLCD.x,directionLCD.y);
}

void draw_entities2D(){
	for(uint8_t i=0;i<MAX_ENTITIES;i++){
		if(entities[i].health>0){
			posLCD entityLCD = coords2LCD(entities[i].pos);
			Buffer_SetPixel(buffer,entityLCD.x,entityLCD.y);
		}
	}
}

void draw2D(){
	Buffer_Reset(buffer);
	draw_map2D();
	draw_player2D();
	draw_entities2D();
}
//--------------------------------------------------------------------------------------------------------------------

void loop(void){
	read_joy();
	
	if(interrupt){
		interrupt=0;
		
		//ADC, ajustar el valor entre 1 y 2pi
		if(read_pot())
			j1.angle=(pot1_read*PI2)/MAX_POT_VALUE;
		
		//mover la posicion del jugador
		update_player();

		//actualizar entidades
		update_entities();
		
		draw2D();
		
		SPILCD_Transfer(spiDrv1, buffer);
	}
}
