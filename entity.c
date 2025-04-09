#include "entity.h"
#include "structure.h"

entity entities[MAX_ENTITIES];
player j1;

//---------------------------------------------------------- JUGADOR ----------------------------------------------------------

//inicializa el jugador en la posicion x,y y con 100 de vida
player set_player(float x, float y){
	player j;
	j.pos = set_coords(x,y);
	j.health=100;
	j.angle=0;
	return j;
}

//actualiza la posicion del jugador en base a la entrada del joystick
//el jugador se mueve en la direccion de la mirada (el angulo cambia con el potenciómetro)
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

//se muestra la salud del jugador en los 4 leds
void display_health(){
    uint8_t led1 = (j1.health >= 25) ? ON : OFF;
    uint8_t led2 = (j1.health >= 50) ? ON : OFF;
    uint8_t led3 = (j1.health >= 75) ? ON : OFF;
    uint8_t led4 = (j1.health >= 100) ? ON : OFF;
    display_leds(led1, led2, led3, led4);
}

//actualizacion del estado del jugador
void update_player(void){

    display_health(); // Actualiza la visualización de salud

    if(j1.health == 0) 
        // El jugador ha muerto, manejar la lógica de muerte aquí
        return;

    move_player(); // Mueve al jugador según la entrada del joystick
}


//---------------------------------------------------------- ENTIDADES ----------------------------------------------------------

//en esta seccion se definen las funciones para crear y actualizar las entidades del juego, ya sean enemigos o healthpacks.

//se define la entidad en una posicion, con 100 de vida, estado idle y el tipo de entidad que se le pase como argumento
entity set_entity(float x, float y, entityType_t type){
    entity e;
    e.pos = set_coords(x,y);
    e.health=100;
    e.state=ENTITY_STATE_IDLE;
    e.type=type;
    return e;
}

void idle_entity(entity *e){
    coords newPos=e->pos;

    newPos.x += e->pos.x + ((rand() % 3) - 1) * ENEMY_SPEED; // Movimiento aleatorio en x
    newPos.y += e->pos.y + ((rand() % 3) - 1) * ENEMY_SPEED; // Movimiento aleatorio en y

    if(!colission(newPos))
        e->pos = newPos;
}

//implementar pathfinding!!!!!
void move_entity(entity *e,coords target,float dist){
    coords newPos = e->pos;
    coords dir = set_coords(target.x - e->pos.x, target.y - e->pos.y);

    if (dist > 0) {
        newPos.x += (ENEMY_SPEED * dir.x) / dist; // Normalizar la dirección
        newPos.y += (ENEMY_SPEED * dir.y) / dist; // Normalizar la dirección
    }

    if (!colission(newPos))
        e->pos = newPos;
}

void update_entity(entity *e){
    float dist = distance(e->pos, j1.pos);

    if(dist > RENDER_DISTANCE)
        return; // No se actualiza si no se ha renderizado

    switch(e->type){
        // SI ES UN ENEMIGO
        case ENTITY_TYPE_ENEMY:

            //si su salud es 0 está muerto
            if(e->health == 0){
                e->state = ENTITY_STATE_DEAD;
                return;
            }
            //se observa el estado del enemigo
            switch (e->state) {
                //si está quieto y dentro del rango de detección, pasa a moverse
                case ENTITY_STATE_IDLE:
                    if (dist < DETECTION_RADIUS){ 
                        e->state = ENTITY_STATE_MOVING;
                    }else{
                        idle_entity(e);
                    }
                    break;
                //si se está moviendo y el jugador está dentro del rango de ataque, pasa a atacar
                //si el jugador está fuera del rango de ataque, pasa a idle
                case ENTITY_STATE_MOVING:
                    if (dist < ATTACK_RADIUS) {
                        e->state = ENTITY_STATE_ATTACKING;
                    }else if(dist>DETECTION_RADIUS){
                        e->state = ENTITY_STATE_IDLE;
                    }else {
                        move_entity(e,j1.pos,dist);
                    }
                    break;
                break;
                
                //si está atacando y el jugador está fuera del rango de ataque, pasa a moverse
                //si el jugador está dentro del rango de ataque, le quita vida al jugador
                case ENTITY_STATE_ATTACKING:
                    if (dist > ATTACK_RADIUS) {
                        e->state = ENTITY_STATE_MOVING;
                    } else {
                        j1.health -= ATTACK;
                    }
                    break;

                case ENTITY_STATE_DEAD:
                    return; // No se actualiza el estado de la entidad muerta
                break;
            }
        break;
        // SI ES UN HEALTHPACK
        case ENTITY_TYPE_HEALTH:
            if (dist < ATTACK_RADIUS) {
                j1.health += HEALTH;
                e->health = 0; // El healthpack se "consume"
                e->state = ENTITY_STATE_DEAD; // Cambia el estado a muerto para que no se actualice más, como si fuese un enemigo muerto

                if(e->state == ENTITY_STATE_DEAD){
                    return;
                }
            }
        break;
    }
}

void update_entities(void){
    for (uint8_t i = 0; i < MAX_ENTITIES; i++)
        update_entity(&entities[i]);
}