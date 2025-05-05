#include "structure.h"
#include "LPC1768_func.h"
#include "Buffer.h"
#include "entity.h"
#include "sprites.h"

#define FOV 64 //angulo en grados del campo de vision del jugador
#define NUM_RAYS (FOV) //un angulo por grado
#define FOV2 (FOV/2)
#define FOV2RAD (FOV2*DEG2RAD)
#define CENTERY (LCD_NUM_ROWS/2)
#define CENTER_Y (LCD_NUM_ROWS/2)
#define RES_DIVIDER 2

#define ANLGE_TOLERANCE 0.1 //tolerancia para el angulo del disparo
#define DISTANCE_MODIFIER 0.8 //modificador de distancia para el escalado de los sprites

uint8_t scene=0; //variable para cambiar entre el menu y el juego
uint8_t shooting = 0;

uint8_t zbuffer[LCD_NUM_COLS]; //se usa para evitar dibujar objetos que esten detras de otros

//---------------------------------------------------------- RAYCASTING ----------------------------------------------------------
//dibuja un rayo en base a la distancia hasta la posicion en el que el rayo colisiona con una pared usando DDA
void renderMap() {
	const float d = (LCD_NUM_ROWS / 2) / tan(FOV2RAD); // Distancia al plano de proyección

    // Casteo de rayos y dibujo de paredes
    for (uint8_t i = 0; i < NUM_RAYS; i++) {
        // Calcula el ángulo para este rayo
        float camera_x = 2 * (double)i / NUM_RAYS - 1;
        float ray_angle = j1.angle + atan(camera_x * tan(FOV2RAD));

        // Inicializa las variables del rayo
        coords rayStart = j1.pos;
        coords rayDir = set_coords(cosf(ray_angle), sinf(ray_angle));
        coords unitStepSize = set_coords(
            sqrt(1 + (rayDir.y / rayDir.x) * (rayDir.y / rayDir.x)),
            sqrt(1 + (rayDir.x / rayDir.y) * (rayDir.x / rayDir.y))
        );

        cell mapCheck = obtainCell(rayStart);
        coords rayLength;
        int8_t stepx, stepy;

        if (rayDir.x < 0) {
            stepx = -1;
            rayLength.x = (rayStart.x - (float)mapCheck.x) * unitStepSize.x;
        } else {
            stepx = 1;
            rayLength.x = ((float)(mapCheck.x + 1) - rayStart.x) * unitStepSize.x;
        }

        if (rayDir.y < 0) {
            stepy = -1;
            rayLength.y = (rayStart.y - (float)mapCheck.y) * unitStepSize.y;
        } else {
            stepy = 1;
            rayLength.y = ((float)(mapCheck.y + 1) - rayStart.y) * unitStepSize.y;
        }

        uint8_t tileFound = 0;
        float distance = 0;

        // Avanzar el rayo hasta encontrar una colisión
        while (tileFound != WALL && distance < LCD_NUM_COLS) {
            if (rayLength.x < rayLength.y) {
                mapCheck.x += stepx;
                rayLength.x += unitStepSize.x;
                distance = rayLength.x;
            } else {
                mapCheck.y += stepy;
                rayLength.y += unitStepSize.y;
                distance = rayLength.y;
            }
            tileFound = colissionCell(mapCheck);
				}
        // Si se encuentra una pared, dibujarla
        if (tileFound == WALL) {
			coords wallHit = set_coords(mapCheck.x+0.5, mapCheck.y+0.5); // Punto de colisión, se toma el centro de la celda para mas precision
			float actualDistance = distance2(j1.pos, wallHit); // Distancia real al objeto colisionado
            uint8_t screenX = i * RES_DIVIDER;
			zbuffer[i] =min((uint8_t)actualDistance,255); // Guardar la distancia en el zbuffer
            uint8_t wallHeight =min((uint8_t)((CELL_SIZE * d) / actualDistance),LCD_NUM_ROWS); // Altura de la pared proyectada

            uint8_t startY = CENTERY - wallHeight / 2;
            uint8_t endY = CENTERY + wallHeight / 2;

            // Dibuja la columna de la pared
            Buffer_DrawLineV(buffer, screenX, startY, endY);

            //rellena los huecos de las paredes
            /*for (uint8_t j = 1; j < RES_DIVIDER; j++) {
                Buffer_DrawLineV(buffer, screenX + j, startY, endY);
            }*/
        }
    }
}


//---------------------------------------------------------- RENDERIZADO DE ENTIDADES ------------------------------------------------------
coords translateIntoView(coords entityPos) {
    // Calcula la posición relativa de la entidad respecto al jugador
	coords sprite = set_coords(entityPos.x - j1.pos.x, entityPos.y - j1.pos.y);

	coords plane = set_coords(-sinf(j1.angle), cosf(j1.angle)); // Vector perpendicular a la dirección del jugador
	coords dir = set_coords(cosf(j1.angle), sinf(j1.angle)); // Vector de dirección del jugador

    // Determina el determinante inverso para la transformación
    float invDet = 1.0f / (plane.x * dir.y - dir.x * plane.y);

    // Transforma las coordenadas al espacio de la cámara
    coords transformed;
    transformed.x = invDet * (dir.y * sprite.x - dir.x * sprite.y); // Coordenada X en el espacio de la cámara
    transformed.y = invDet * (-plane.y * sprite.x + plane.x * sprite.y); // Coordenada Z en el espacio de la cámara

    return transformed;
}

void drawImpSprite(uint8_t spriteIndex, uint8_t x, uint8_t y, float distance) {
    // Verifica que el índice del sprite esté dentro del rango válido
    if (spriteIndex >= BMP_IMP_COUNT) {
        return; // Índice fuera de rango, no hace nada
    }

    // Calcula el desplazamiento del sprite dentro del bitmap
    uint16_t offset = spriteIndex * (BMP_IMP_WIDTH * BMP_IMP_HEIGHT / 8);
    const uint8_t *spriteData = &bmp_imp_bits[offset];
    const uint8_t *maskData = &bmp_imp_mask[offset];

    // Dibuja el sprite en la posición especificada
    Buffer_DrawSprite(buffer, zbuffer, spriteData, maskData, BMP_IMP_WIDTH, BMP_IMP_HEIGHT, x, y, distance);
}

void renderEntities(){
	for(uint8_t i=0;i<numEnemies;i++)
		if(enemies[i].state != ENEMY_STATE_INACTIVE){
			coords cameraPos = translateIntoView(enemies[i].base.pos); // Transforma la posición de la entidad al espacio de la cámara
			if(cameraPos.y > 0){ // Solo dibuja si la entidad está delante del jugador
				uint8_t screenX = (LCD_NUM_COLS / 2) * (1 + cameraPos.x / cameraPos.y);

				// Dibuja el sprite de la entidad
				zbuffer[screenX] = min(zbuffer[screenX],enemies[i].base.dist); // Guardar la distancia en el zbuffer
				uint8_t truncatedDistance = max(1,(uint8_t)(enemies[i].base.dist*DISTANCE_MODIFIER)); // Modifica la distancia para el escalado del sprite
				drawImpSprite(enemies[i].spriteIndex, screenX, CENTERY, truncatedDistance); // Dibuja el sprite del enemigo
			}
		}

	for(uint8_t i=0;i<numHealthpacks;i++)
		if(healthpacks[i].base.health > 0){
			coords cameraPos = translateIntoView(healthpacks[i].base.pos); // Transforma la posición de la entidad al espacio de la cámara

			if(cameraPos.y > 0){ // Solo dibuja si la entidad está delante del jugador
				uint8_t screenX = (LCD_NUM_COLS / 2) * (1 + cameraPos.x / cameraPos.y);

				// Dibuja el sprite de la entidad
				zbuffer[screenX] = min(zbuffer[screenX],healthpacks[i].base.dist); // Guardar la distancia en el zbuffer
				uint8_t truncatedDistance = max(1,(uint8_t)healthpacks[i].base.dist);
				Buffer_DrawSprite(buffer, zbuffer,bmp_items_bits,bmp_items_mask,BMP_ITEMS_WIDTH, BMP_ITEMS_HEIGHT, screenX, CENTERY,truncatedDistance);
			}
		}

	for(uint8_t i=0;i<numFireballs;i++)
		if(fireballs[i].base.health > 0){
			coords cameraPos = translateIntoView(fireballs[i].base.pos); // Transforma la posición de la entidad al espacio de la cámara

			if(cameraPos.y > 0){ // Solo dibuja si la entidad está delante del jugador
				uint8_t screenX = (LCD_NUM_COLS / 2) * (1 + cameraPos.x / cameraPos.y);

				// Dibuja el sprite de la entidad
				zbuffer[screenX] = min(zbuffer[screenX],fireballs[i].base.dist); // Guardar la distancia en el zbuffer
				uint8_t truncatedDistance = max(1,(uint8_t)fireballs[i].base.dist);
				Buffer_DrawSprite(buffer, zbuffer,bmp_fireball_bits,bmp_fireball_mask,BMP_FIREBALL_WIDTH, BMP_FIREBALL_HEIGHT, screenX, CENTERY,truncatedDistance);
			}
		}
}

void renderGun(){
	if(shooting){
		Buffer_DrawMenu(buffer, Doom_Gun_Firing, 20,30,54,5); //sprite pistola fuego
	}else
		Buffer_DrawMenu(buffer, Doom_Gun, 20,15,54,18); //si no se está disparando se muestra la pistola estática
}

void shoot(){
	for(uint8_t i=0;i<numEnemies;i++)
		if(enemies[i].state != ENEMY_STATE_INACTIVE && enemies[i].state != ENEMY_STATE_DEAD){
			coords transform = translateIntoView(enemies[i].base.pos); // Transforma la posición de la entidad al espacio de la cámara
			if(transform.y>0){
				float angle = atan2f(transform.x,transform.y); // Calcula el ángulo entre el jugador y la entidad
				if(fabsf(angle) < ANLGE_TOLERANCE){ // Si el ángulo es menor a la tolerancia, se considera un disparo exitoso
					// Disparo exitoso
					enemies[i].base.health = max(0, enemies[i].base.health - PLAYER_ATTACK); // Resta vida al enemigo
					enemies[i].state = ENEMY_STATE_HIT;
					enemies[i].timer = 4;
				}
			}
		}
}

//---------------------------------------------------------- 2D ------------------------------------------------------
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
	p2.x = p1.x + dist*cosf(angle);
	p2.y = p1.y + dist*sinf(angle);
	return p2;
}
void draw_player2D(){
	posLCD playerLCD = coords2LCD(j1.pos);
	posLCD directionLCD = coords2LCD(hallar_puntoDireccion(j1.pos,j1.angle,3));
	Buffer_SetPixel(buffer,playerLCD.x,playerLCD.y);
	//Buffer_SetPixel(buffer,directionLCD.x,directionLCD.y);
}

void draw_entities2D(){
	for(uint8_t i=0;i<numEnemies;i++){
		if(enemies[i].state != ENEMY_STATE_INACTIVE){
			posLCD enemyLCD = coords2LCD(enemies[i].base.pos);
			Buffer_SetPixel(buffer,enemyLCD.x,enemyLCD.y);
		}
	}

	for(uint8_t i=0;i<numHealthpacks;i++){
		if(healthpacks[i].base.health > 0){
			posLCD healthpackLCD = coords2LCD(healthpacks[i].base.pos);
			Buffer_SetPixel(buffer,healthpackLCD.x,healthpackLCD.y);
		}
	}

	for(uint8_t i=0;i<numFireballs;i++){
		if(fireballs[i].base.health > 0){
			posLCD fireballLCD = coords2LCD(fireballs[i].base.pos);
			Buffer_SetPixel(buffer,fireballLCD.x,fireballLCD.y);
		}
	}
}

void draw2D(){
	Buffer_Reset(buffer);
	draw_map2D();
	draw_player2D();
	draw_entities2D();
}


//---------------------------------------------------------- MENU Y GAMEPLAY ------------------------------------------------------

void loop_gameplay(void){
	read_joy();
	
	if(interrupt){
		interrupt=0;

		if(killCount == numEnemies)
			display_rgb(RGB_OFF,RGB_ON,RGB_OFF); //si se han matado todos los enemigos, el rgb parpadea en verde
		
		//actualizar entidades
		update_entities();
		if(j1.health == 0){
			display_leds(OFF,OFF,OFF,OFF); //apaga los leds
			scene=0; //si el jugador muere se vuelve al menu
			return;
		}
		
				//si se presiona el boton central se dispara
		//si se ha disparado se espera a que se reduzca el timer
		if(j1.timer){
			j1.timer--;
		}else
			shooting = 0;
		//borrar flag
		if(center_registered){
			center_registered=0;
			
			//ya se puede volver a disparar
			if(j1.timer==0){
				j1.timer=PLAYER_TIMER; //timer para evitar disparos continuos
				shooting=1;
				shoot();
			}
		}
		
		Buffer_Reset(buffer);
		ZBuffer_Reset(zbuffer);
		renderMap();
		renderEntities();
		renderGun();
		//draw2D();
		SPILCD_Transfer(spiDrv1, buffer);
	}
}



void loop_menu(void){
	spawn_entities();
	Buffer_Reset(buffer);
	Buffer_DrawMenu(buffer,Doom_logo_LN, 128, 32, 0, 0); //muestra el logo del Doom
	SPILCD_Transfer(spiDrv1, buffer);
	while(!center_registered){
			read_center();//espera a que se presione el joystick central
	}
	scene=1; //cambia a la escena de juego
	center_registered=0; //reinicia el registro del joystick central
}

void loop(void){
	if(scene){
		loop_gameplay();
	}else
		loop_menu();
}
