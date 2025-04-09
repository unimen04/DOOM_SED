#include "structure.h"

entity entities[MAX_ENTITIES];
player j1;

player set_player(float x, float y){
	player j;
	j.pos = set_coords(x,y);
	j.health=100;
	j.angle=0;
	return j;
}

entity set_entity(float x, float y, entityType_t type){
    entity e;
    e.pos = set_coords(x,y);
    e.health=100;
    e.state=ENTITY_STATE_IDLE;
    e.type=type;
    e.renderized=0;
    return e;
}

void idle_entity(entity *e){
    // Implementar la lógica para el estado idle
   if(e->state != ENTITY_STATE_IDLE)
        return;
    
    //inicializar la semilla si aun no se ha hecho(buscar una forma mas optima de implementar esto)
    static uint8_t seed=0;
    if(seed==0){
        seed = 1;
        srand(time(NULL)); // Inicializa la semilla de aleatoriedad
    }
    
    coords newPos;
    newPos.x = e->pos.x + ((rand() % 3) - 1) * ENEMY_SPEED; // Movimiento aleatorio en x
    newPos.y = e->pos.y + ((rand() % 3) - 1) * ENEMY_SPEED; // Movimiento aleatorio en y

    if(!colission(newPos))
        e->pos = newPos;
}

//implementar pathfinding!!!!!
void move_entity(entity *e,coords target){
}

void update_entity(entity *e){
    float dist = distance(e->pos, j1.pos);

    switch(e->type){
        case ENEMY:
            if(!entity->renderized)
                return; // No se actualiza si no se ha renderizado

            if(e->health == 0){
                e->state = ENTITY_STATE_DEAD;
                return;
            }

            switch (e->state) {
            case ENTITY_STATE_IDLE:
                if (dist < DETECTION_RADIUS){ 
                    e->state = ENTITY_STATE_MOVING;
                }else{
                    idle_entity(e);
                }
                break;
            
            case ENTITY_STATE_MOVING:
                if (dist < ATTACK_RADIUS) {
                    e->state = ENTITY_STATE_ATTACKING;
                }else if(dist>DETECTION_RADIUS){
                    e->state = ENTITY_STATE_IDLE;
                }else {
                    move_entity(e,j1.pos);
                }
                break;
            break;

            case ENTITY_STATE_ATTACKING:
                if (dist > ATTACK_RADIUS) {
                    e->state = ENTITY_STATE_MOVING;
                } else {
                    j1.health -= ATTACK;
                }
                break;

            case ENTITY_STATE_DEAD;
                return; // No se actualiza el estado de la entidad muerta
            break;
            }
        break;

        case HEALTHPACK:
            if (dist < ATTACK_RADIUS) {
                j1.health += HEALTH;
                e->health = 0; // El healthpack se "consume"
            }
        break;
    }
}

//renderiza una entidad dada su posicion
void renderize_entity(coords entityPos){
    for(uint8_t i=0;i<MAX_ENTITIES;i++){
        if(entities[i].pos.x==entityPos.x && entities[i].pos.y==entityPos.y){
            entities[i].renderized=1;
            return;
        }
    }
}

void update_entities(void){
    for (uint8_t i = 0; i < MAX_ENTITIES; i++)
        update_entity(&entities[i]);
}