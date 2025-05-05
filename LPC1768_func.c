#include "LPC1768_func.h"

//LCD
//Puntero al driver SPI que utilizará la pantalla LCD
ARM_DRIVER_SPI* spiDrv1 = &Driver_SPI1;
//flag para indicar si hay que actualizar la pantalla
uint8_t updateLcd=0;
//buffer de la pantalla
uint8_t buffer[LCD_MEM_SIZE];

//JOYSTICKS
//estados de los diferentes botones del joystick
uint8_t center_pressed=0;
uint8_t center_pressed_prev=0;
uint8_t center_registered=0;
uint8_t up_pressed=0;
uint8_t down_pressed=0;
uint8_t right_pressed=0;
uint8_t left_pressed=0;

//ADC
//variables para almacenar el valor devuelto por los potenciometros
uint32_t pot1_read=0;
uint32_t pot2_read=0;

//Timer
//flag de interrupcion
uint8_t interrupt=0;

//---------------------------------------------------------- JOYSTICKS ----------------------------------------------------------
void initPin_JOY(uint32_t port_number, uint32_t pin_number)
{
	// configura el pin como GPIO de entrada con resistencia de pulldown
	GPIO_SetDir(port_number, pin_number, GPIO_DIR_INPUT);
	PIN_Configure(port_number, pin_number, PIN_FUNC_0,
	              PIN_PINMODE_PULLDOWN, PIN_PINMODE_NORMAL);
}

//lee el centro del joystick y lo registra si se ha presionado
void read_center(){
	center_pressed_prev=center_pressed;
	center_pressed=GPIO_PinRead(PORT_JOY, PIN_JOY_CENTER);
	if(center_pressed && !center_pressed_prev)
		center_registered=1;
}

//lee todos los joysticks
void read_joy(){
	read_center();

	if(GPIO_PinRead(PORT_JOY, PIN_JOY_CENTER)){
		center_pressed=1;
	}else if(GPIO_PinRead(PORT_JOY, PIN_JOY_UP)){
		up_pressed=1;
	}else if(GPIO_PinRead(PORT_JOY, PIN_JOY_DOWN)){
		down_pressed=1;
	}else if(GPIO_PinRead(PORT_JOY, PIN_JOY_LEFT)){
		left_pressed=1;
	}else if(GPIO_PinRead(PORT_JOY, PIN_JOY_RIGHT)){
		right_pressed=1;
	}
			
}

//---------------------------------------------------------- LEDS ----------------------------------------------------------
void initPin_LED(uint32_t port_number, uint32_t pin_number)
{
	// configura el pin como GPIO de salida con resistencia de pulldown
	GPIO_SetDir(port_number, pin_number, GPIO_DIR_OUTPUT);
	PIN_Configure(port_number, pin_number, PIN_FUNC_0,
	              PIN_PINMODE_PULLDOWN, PIN_PINMODE_NORMAL);
}

void display_leds(uint8_t led1, uint8_t led2, uint8_t led3, uint8_t led4){
	GPIO_PinWrite(PORT_LED, PIN_LED1, led1);
	GPIO_PinWrite(PORT_LED, PIN_LED2, led2);
	GPIO_PinWrite(PORT_LED, PIN_LED3, led3);
	GPIO_PinWrite(PORT_LED, PIN_LED4, led4);
}

//---------------------------------------------------------- RGB ----------------------------------------------------------
void initPin_RGB(uint32_t port_number, uint32_t pin_number)
{
	// configura el pin como GPIO de salida con resistencia de pulldown
	GPIO_SetDir(port_number, pin_number, GPIO_DIR_OUTPUT);
	PIN_Configure(port_number, pin_number, PIN_FUNC_0,
	              PIN_PINMODE_PULLDOWN, PIN_PINMODE_NORMAL);
}

void display_rgb(uint8_t red, uint8_t green, uint8_t blue){
	GPIO_PinWrite(PORT_RGB, PIN_RGB_RED, red);
	GPIO_PinWrite(PORT_RGB, PIN_RGB_GREEN, green);
	GPIO_PinWrite(PORT_RGB, PIN_RGB_BLUE, blue);
}



//---------------------------------------------------------- TIMER ----------------------------------------------------------
void init_timer(int maxCount){
	LPC_TIM0->MCR |= 1 << 0; 	 //genera interrupciones
	LPC_TIM0->MCR |= 1 << 1; 	 //reinicia el contador cuando llegue a su valor maximo
	LPC_TIM0->PR=0;				 //valor del prescaler
	LPC_TIM0->MR0=maxCount;		 //registro de comparacion del timer
	NVIC_EnableIRQ(TIMER0_IRQn); //activa las interrupciones
	LPC_TIM0->TCR |= 1 << 0;	 //activa el contador	
}
//---------------------------------------------------------- ADC ----------------------------------------------------------
void init_ADC(void){
	//activa el ADC
	LPC_SC->PCONP |= (1 << 12);
	
	//configura ambos potenciometros con funcion 3 (ADC)
	PIN_Configure(PORT_POT, POT1, PIN_FUNC_3,
	              PIN_PINMODE_PULLDOWN, PIN_PINMODE_NORMAL);
	PIN_Configure(PORT_POT, POT2, PIN_FUNC_3,
	              PIN_PINMODE_PULLDOWN, PIN_PINMODE_NORMAL);
	
	//reset de SEL y CLKDIV
	LPC_ADC->ADCR &= (0xFF<<0);
	LPC_ADC->ADCR &= (0xFF<<SBIT_CLCKDIV);
	
	LPC_ADC->ADCR |= ((0x1 << CANAL1) | (0x1<<CANAL2));	//selecciona los canales 4 y 5 del ADC
	LPC_ADC->ADCR |= (0x4 << SBIT_CLCKDIV);				//Divide el clk por 5
	LPC_ADC->ADCR |= (0x1 << SBIT_PDN);					//Habilita el conversor
	LPC_ADC->ADCR &= ~(0x0 << SBIT_START);				//Asegurar que el conversor esta parado

}

uint32_t read_ADC(uint8_t canal, uint8_t canal_previo){
	uint32_t valor;
	if(canal_previo)
		LPC_ADC->ADCR &= ~(0x1F << canal_previo);
	LPC_ADC->ADCR |= (0x1 << canal); 
	LPC_ADC->ADCR |= 0x001<<SBIT_START;            		// Start ADC conversion
	while((LPC_ADC->ADGDR & (1UL<<(SBIT_DONE)))== 0)
		;	// Wait till conversion completes 
    valor = (LPC_ADC->ADGDR >> SBIT_RESULT) & 0xfff;    // Seleccionar los bits del resultado
	return valor;
}

//devuelve 1 si ha modificado el valor del potenciometro
uint8_t read_pot(void){
	uint32_t prev_potValue=pot1_read;
	uint32_t newPotValue = read_ADC(CANAL1,0);
	if(prev_potValue-newPotValue>DIFF){
		pot1_read=newPotValue;
		return 1;
	}
	return 0;
	//pot2_read=read_ADC(CANAL2,CANAL1); solo se usa un potenciometro
}


//---------------------------------------------------------- START ----------------------------------------------------------

void start(void){

	// Initialize system
	SystemInit();
	GPIO_PortClock(1);
	
	// Initialize, reset and configure LCD
	SPILCD_Init(spiDrv1);
	SPILCD_Reset(spiDrv1);
	SPILCD_Configure(spiDrv1);

	// Reset buffer
	Buffer_Reset(buffer);
	
	//inicializa el adc
	init_ADC();
	
	//inicializa el timer
	init_timer(MAXCOUNT);
	
	//inicializa los joysticks
	initPin_JOY(PORT_JOY, PIN_JOY_CENTER);
	initPin_JOY(PORT_JOY, PIN_JOY_UP);
	initPin_JOY(PORT_JOY, PIN_JOY_DOWN);
	initPin_JOY(PORT_JOY, PIN_JOY_RIGHT);
	initPin_JOY(PORT_JOY, PIN_JOY_LEFT);

	//inicializa los leds
	initPin_LED(PORT_LED, PIN_LED1);
	initPin_LED(PORT_LED, PIN_LED2);
	initPin_LED(PORT_LED, PIN_LED3);
	initPin_LED(PORT_LED, PIN_LED4);
	
	//inicializa el rgb
	initPin_RGB(PORT_RGB, PIN_RGB_RED);
	initPin_RGB(PORT_RGB, PIN_RGB_GREEN);
	initPin_RGB(PORT_RGB, PIN_RGB_BLUE);
	display_rgb(RGB_OFF,RGB_OFF,RGB_OFF); //apaga el rgb al inicio
}
