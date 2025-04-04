#include "LPC1768_func.h"
#include "DOOM.h"

//interrupcion del timer
void TIMER0_IRQHandler(void){
	interrupt=1;
	LPC_TIM0->IR |= 1 << 0;
}

int main(){
	//inicializa todo lo que tenga que ver con la placa
  	start();
	
	//inicializa el juego
	init_game();
	
	while(1) loop(); //loop principal del juego
}
