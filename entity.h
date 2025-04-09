#ifndef ENTITY_H
#define ENTITY_H

#include "structure.h"
#include <time.h>

//velocidad del jugador en celdas/frame
#define PLAYER_SPEED 0.2
#define ENEMY_SPEED 0.1

#define HEALTH 100
#define ATTACK 20

#define DETECTION_RADIUS 5
#define ATTACK_RADIUS 1

#define MAX_ENTITIES 10

typedef enum entityState {
    ENTITY_STATE_IDLE,
    ENTITY_STATE_MOVING,
    ENTITY_STATE_ATTACKING,
    ENTITY_STATE_DEAD
} entityState_t;

typedef enum entityType {
    ENTITY_TYPE_ENEMY,
    ENTITY_TYPE_HEALTH
} entityType_t;

typedef struct{
    coords pos;
    uint8_t health;
    entityState_t state;
    entityType_t type;
    uint8_t renderized;
} entity;

typedef struct{
	coords pos;
	uint8_t health;
	float angle;
} player;

extern entity entities[MAX_ENTITIES];
extern player j1;

player set_player(float x, float y);

entity set_entity(float x, float y);

void renderize_entity(coords pos);

void update_entities(void);

#endif
