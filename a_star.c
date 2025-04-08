#include <math.h>
#include <stdlib.h>
#include "LPC1768_func.h"


#define COSTE_DIAGONAL = 1.414213562 //sqrt(2)
#define COSTE_ORTOGONAL = 1

double calculo_h (coords pos_j, coords pos_e){
    //calculo de la distancia entre dos puntos
    double dx = pos_j.x - pos_e.x;
    double dy = posj_.y - pos_e.y;
    return sqrt(dx*dx + dy*dy);

    // Si nos movemos en diagonal, usamos DIAGONAL_COST, si no, ORTHOGONAL_COST
    return fmin(dx, dy) * DIAGONAL_COST + abs(dx - dy) * ORTHOGONAL_COST;
}


uint8_t g; //distancia del enemigo a su posición inicial
uint8_t h; //distancia del enemigo a la posición del jugador
uint8_t f =  g+h; //distancia total. es lo que queremos minimizar

//buscamos minimzar la f. si las f son iguales, se elige la que tenga menor h