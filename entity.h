#ifndef ENTITY_H
#define ENTITY_H

#include "structure.h"
#include "LPC1768_func.h"
#include <time.h>
#include <stdlib.h>

//velocidad de cada entidad en celdas/frame
#define PLAYER_SPEED 0.3
#define FIREBALL_SPEED 0.25
#define ENEMY_SPEED 0.1

#define HEALTH 100
#define HEALTH_RESTORE 50
#define FIREBALL_DAMAGE 20
#define ATTACK 25
#define PLAYER_ATTACK 25

#define RENDER_DISTANCE 20
#define DETECTION_RADIUS 10
#define ATTACK_RADIUS 2
#define RANGED_ATTACK_RADIUS 5
#define PICK_RADIUS 0.6
#define ENEMY_MELEE_TIMER 20
#define ENEMY_RANGED_TIMER 50
#define PLAYER_TIMER 15

#define MAX_ENTITIES 15

typedef enum enemyState {
    ENEMY_STATE_IDLE,
    ENEMY_STATE_MOVING,
    ENEMY_STATE_ATTACKING,
    ENEMY_STATE_RANGED_ATTACKING,
    ENEMY_STATE_HIT,
    ENEMY_STATE_DEAD,
    ENEMY_STATE_INACTIVE
} enemyState_t;

typedef struct{
    coords pos;
    uint8_t health;
    float dist; // Distancia al jugador
} entity_t;

typedef struct{
    entity_t base; // Datos básicos de la entidad
    enemyState_t state;
    uint8_t spawned; // Indica si el enemigo ha sido creado
    uint8_t timer; // Timer del enemigo
    uint8_t spriteIndex; // Indice del sprite del enemigo
} enemy_t;

typedef struct{
    entity_t base; // Datos básicos de la entidad
    uint8_t spawned; // Indica si el healthpack ha sido creado
    uint8_t health_amount; // Vida del healthpack
} healthpack_t;

typedef struct{
    entity_t base; // Datos básicos de la entidad
    coords direction; // Dirección de la bola de fuego
} fireball_t;


typedef struct{
	coords pos;
	int8_t health;
	float angle;
  uint8_t timer; // Timer para evitar ataques continuos
} player;

extern player j1;

extern enemy_t enemies[MAX_ENTITIES];
extern healthpack_t healthpacks[MAX_ENTITIES];
extern fireball_t fireballs[MAX_ENTITIES];

extern uint8_t numEnemies;
extern uint8_t numHealthpacks;
extern uint8_t numFireballs;
extern uint8_t killCount; // Contador de enemigos muertos

void spawn_entities(void);
void update_entities(void);
#endif
