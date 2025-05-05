#include "entity.h"
#include "structure.h"

enemy_t enemies[MAX_ENTITIES];
healthpack_t healthpacks[MAX_ENTITIES];
fireball_t fireballs[MAX_ENTITIES];

uint8_t numEnemies;
uint8_t numHealthpacks;
uint8_t numFireballs = 0;
uint8_t killCount=0;

player j1;

//---------------------------------------------------------- JUGADOR ----------------------------------------------------------

//inicializa el jugador en la posicion x,y y con 100 de vida
player set_player(float x, float y){
	player j;
	j.pos = set_coords(x,y);
	j.health=HEALTH;
	j.angle=0;
    j.timer=0; // Timer para evitar ataques continuos
	return j;
}

//actualiza la posicion del jugador en base a la entrada del joystick
//el jugador se mueve en la direccion de la mirada (el angulo cambia con el potenciómetro)
void move_player(){
	volatile coords newPos = j1.pos;
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
	if(colission(newPos) != 1)
		j1.pos=newPos;
	volatile coords a = j1.pos;
}

//se muestra la salud del jugador en los 4 leds
void display_health(){
    uint8_t led1 = (j1.health > 0) ? ON : OFF;
    uint8_t led2 = (j1.health >= 50) ? ON : OFF;
    uint8_t led3 = (j1.health >= 75) ? ON : OFF;
    uint8_t led4 = (j1.health >= 100) ? ON : OFF;
    display_leds(led1, led2, led3, led4);
}

//actualizacion del estado del jugador
void update_player(void){
    if(j1.health < 0)
        j1.health = 0; //asegura que la salud no sea negativa

    display_health(); //actualiza la visualización de salud
    //ADC, ajustar el angulo del jugador a un valor entre 1 y 2pi
	if(read_pot())
        j1.angle=(pot1_read*PI2)/MAX_POT_VALUE;

    move_player(); //mueve al jugador según la entrada del joystick
}


//---------------------------------------------------------- ENTIDADES: Estructuras ----------------------------------------------------------

//las distancias siempre se dejan a 0 al inicializar porque se calculan cada vez q se actualizan las entidades

//estructura del enemigo
enemy_t set_enemy(float x, float y){
    enemy_t e;
    e.base.pos = set_coords(x,y); // Posición inicial del enemigo
    e.base.health = 100;
    e.base.dist = 0;
    e.spawned = 1;
    e.state = ENEMY_STATE_IDLE;
    e.timer = 5; // Timer para evitar ataques continuos
    e.spriteIndex = 1; // Indice del sprite del enemigo
    
    numEnemies++; // Incrementar el contador de enemigos
    if(numEnemies > MAX_ENTITIES) numEnemies = 0;
    return e;
}

//estructura del healthpack
healthpack_t set_healthpack(float x, float y){
    healthpack_t h;
    h.base.pos = set_coords(x,y); // Posición inicial del healthpack
    h.base.health = 1; //vida del healthpack, 1 para que se destruya al tocar al jugadro
    h.base.dist = 0; //distancia inicial
    h.spawned = 1;
    h.health_amount = 50; //cantidad de vida que otorga al jugador

    numHealthpacks++; //incrementar el contador de healthpacks
    if(numHealthpacks > MAX_ENTITIES) numHealthpacks = 0;
    return h;
}

//estructura del fireball
fireball_t set_fireball(coords pos){
    fireball_t f;
    f.base.pos = pos;
    f.base.health = 1; //vida del fireball, 1 para que se destruya al impactar
    f.base.dist = 0; //distancia inicial

    coords pDir = set_coords(j1.pos.x - f.base.pos.x, j1.pos.y - f.base.pos.y); //dirección hacia el jugador
    float magnitude = sqrt(pDir.x * pDir.x + pDir.y * pDir.y); // Magnitud del vector dirección
    if(magnitude > 0){
        f.direction.x = pDir.x / magnitude; // Normaliza la dirección
        f.direction.y = pDir.y / magnitude; // Normaliza la dirección
    } 

    numFireballs++; // Incrementar el contador de fireballs
    if(numFireballs > MAX_ENTITIES) numFireballs = 0;
    return f;
}

//---------------------------------------------------------- ENTIDADES: Movimiento ----------------------------------------------------------

void move_enemy(enemy_t *e){
    coords pDir = set_coords(j1.pos.x - e->base.pos.x, j1.pos.y - e->base.pos.y); // Dirección hacia el jugador
    coords newPos = e->base.pos;
    
    // Mueve al enemigo en la dirección del jugador
    if(e->base.dist > 0){
        newPos.x += ENEMY_SPEED * pDir.x/e->base.dist; // Normaliza la dirección
        newPos.y += ENEMY_SPEED * pDir.y/e->base.dist; // Normaliza la dirección
    }

    // Solo se actualiza la posición si no hay colisión
    if(colission(newPos) != WALL)
        e->base.pos = newPos;
}

void update_enemy(enemy_t *e){
    if(e->state == ENEMY_STATE_INACTIVE) return; // Si el enemigo ha despawneado, no hacer nada
    e->base.dist = distance2(e->base.pos,j1.pos); // Actualiza la distancia al jugador
    if(e->base.dist>RENDER_DISTANCE) return; // Si la entidad está inactiva o supera la distancia de renderizado, no hacer nada

    switch(e->state){
        case ENEMY_STATE_IDLE:
            e->spriteIndex = 1; // Cambia el sprite a idle
            if(e->base.dist < DETECTION_RADIUS){
                e->state = ENEMY_STATE_MOVING;
                e->timer = 10;
            }
        break;

        case ENEMY_STATE_MOVING:
            //Asigna el sprite dependiendo del timer
            if(e->timer == 0)
                e->timer = 10; //timer para manejar el sprite
						e->spriteIndex = e->timer%2 + 1; // Cambia el sprite entre 1 y 2

            //asigna el proximo estado del enemigo
            if(e->base.dist > DETECTION_RADIUS){
                e->state = ENEMY_STATE_IDLE;
            }else if(e->base.dist < RANGED_ATTACK_RADIUS)
                e->timer = ENEMY_RANGED_TIMER;
                e->state = ENEMY_STATE_RANGED_ATTACKING;

            move_enemy(e); // Mueve al enemigo hacia el jugador
        break;

        case ENEMY_STATE_RANGED_ATTACKING:
            if(e->base.dist > RANGED_ATTACK_RADIUS){
                e->state = ENEMY_STATE_MOVING;
                e->timer = 10;
            }else if(e->base.dist < ATTACK_RADIUS){
								e->timer = ENEMY_MELEE_TIMER; // Timer para evitar ataques continuos
                e->state = ENEMY_STATE_ATTACKING;
            }else if(e->timer == 0){
                e->timer = ENEMY_RANGED_TIMER; // Timer para evitar ataques continuos
                fireballs[numFireballs] = set_fireball(e->base.pos); // Crea un fireball en la posición del enemigo
                e->spriteIndex = 2; // Cambia el sprite a disparando
            }else
                e->spriteIndex = 1; // Cambia el sprite a idle
        break;

        case ENEMY_STATE_ATTACKING:
            if(e->base.dist > ATTACK_RADIUS){
                e->state = ENEMY_STATE_RANGED_ATTACKING;
                e->timer = ENEMY_RANGED_TIMER;
            }else if(e->timer == 0){
                e->timer = ENEMY_MELEE_TIMER; // Timer para evitar ataques continuos
                e->spriteIndex = 2; // Cambia el sprite a atacando
                if(j1.health > 0)
                    j1.health -= ATTACK; // Resta vida al jugador
            }else
                e->spriteIndex = 1; // Cambia el sprite a idle
        break;

        case ENEMY_STATE_HIT:
            e->spriteIndex = 3; // Cambia el sprite a golpeado

            if(e->base.health == 0){ // Si la salud del enemigo es 0, cambiar a estado muerto
                e->state = ENEMY_STATE_DEAD;
                e->spriteIndex = 4; // Cambia el sprite a muerto
                e->timer = 255; //Timer hasta que la entidad pase a inactiva
                killCount++; // Incrementa el contador de enemigos muertos
            }else if(e->timer == 0)
                e->state = ENEMY_STATE_IDLE; // Regresa al estado idle después de ser golpeado
        break;

        case ENEMY_STATE_DEAD:
            if(e->timer == 0)
                e->state = ENEMY_STATE_INACTIVE; // Desactivar el enemigo después de despawnear
        break;
    }
    e->timer--; // Decrementa el timer del enemigo
}

void updateHealthpack(healthpack_t *h){
    if(h->base.health == 0) return; // Si el healthpack ha sido recogido, no hacer nada
    h->base.dist = distance2(h->base.pos,j1.pos); // Actualiza la distancia al jugador
    if(h->base.dist>RENDER_DISTANCE) return; // Si el healthpack está fuera de rango, no hacer nada

    if( h->base.dist < PICK_RADIUS && j1.health < HEALTH){
        j1.health =min(HEALTH,j1.health+h->health_amount); // Aumenta la vida del jugado
        h->base.health = 0; // Marcar el healthpack como recogido
    }
}

void move_fireball(fireball_t *f){
    coords newPos = f->base.pos;
    if(f->base.dist > 0){
        newPos.x += FIREBALL_SPEED * f->direction.x; 
        newPos.y += FIREBALL_SPEED * f->direction.y; 
    }

    if(colission(newPos)==WALL){
        f->base.health = 0; // Matar el fireball si colisiona con una pared
    }else
        f->base.pos = newPos; // Actualiza la posición del fireball si no hay colisión
}

void update_fireball(fireball_t *f){
    if(f->base.health == 0) return; // Si el fireball ha sido destruido, no hacer nada
    f->base.dist = distance2(f->base.pos,j1.pos); // Actualiza la distancia al jugador
    if(f->base.dist < PICK_RADIUS){
        if(j1.health > 0)
            j1.health -= FIREBALL_DAMAGE; // Resta vida al jugador
        f->base.health = 0; // Matar el fireball
    }else
        move_fireball(f); // Mueve el fireball hacia el objetivo (jugador)
}

//---------------------------------------------Funciones de spawn y update de entidades--------------------------------------------------

//se encarga de inicializar al jugador
void spawn_entities(void){
    numEnemies = 0; // Inicializa el contador de enemigos
    numHealthpacks = 0; // Inicializa el contador de healthpacks
    for(uint8_t i=0; i<MAP_HEIGHT; i++)
        for(uint8_t j=0; j<MAP_WIDTH; j++)
            switch(map[i][j]){
                case PLAYER:
                    j1 = set_player(j,i); // Inicializa el jugador en la posición del mapa
                break;

                case ENEMY:
                    enemies[numEnemies] = set_enemy(j,i); // Inicializa el enemigo en la posición del mapa
                break;

                case HEALTHPACK:
                    healthpacks[numHealthpacks] = set_healthpack(j,i); // Inicializa el healthpack en la posición del mapa
                break;
            }
}

//se encarga de actualizar el estado de todas las entidades una vez por interrupcion
void update_entities(void){
    update_player(); // Actualiza el estado del jugador

    //actualiza el estado de los enemigos, healthpacks y fireballs
    for(uint8_t i=0; i < numEnemies; i++)
        update_enemy(&enemies[i]);

    for(uint8_t i=0; i < numHealthpacks; i++)
        updateHealthpack(&healthpacks[i]);

    for(uint8_t i=0; i < numFireballs; i++)
        update_fireball(&fireballs[i]);
}
