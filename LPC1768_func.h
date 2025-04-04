#ifndef LPC1768_func
#define LPC1768_func

#include <LPC17xx.h>
#include <PIN_LPC17xx.h>
#include <GPIO_LPC17xx.h>
#include "SPI_LCD.h"
#include "Buffer.h"

//ADC
#define PORT_POT 1
#define POT1 30
#define POT2 31
#define CANAL1 4
#define CANAL2 5
#define POR_MAX_VALUE 4095 //valor maximo del potenciometro

#define SBIT_BURST      16u
#define SBIT_START      24u
#define SBIT_PDN        21u
#define SBIT_EDGE       27u 
#define SBIT_DONE       31u
#define SBIT_RESULT     4u
#define SBIT_CLCKDIV    8u

//Timer
#define MAXCOUNT 2250000

//Joysticks
#define PORT_JOY 0
#define PIN_JOY_UP 23
#define PIN_JOY_DOWN 17
#define PIN_JOY_LEFT 15
#define PIN_JOY_RIGHT 24
#define PIN_JOY_CENTER 16

//LCD
extern ARM_DRIVER_SPI Driver_SPI1;
extern ARM_DRIVER_SPI* spiDrv1;

extern uint8_t updateLcd;
extern uint8_t buffer[LCD_MEM_SIZE];

//Joysticks
extern uint8_t center_pressed;
extern uint8_t up_pressed;
extern uint8_t down_pressed;
extern uint8_t right_pressed;
extern uint8_t left_pressed;

//ADC
extern uint32_t pot1_read;
extern uint32_t pot2_read;

//Timer
extern uint8_t interrupt;

void read_joy(void);

void init_timer(int maxCount);

void stop_timer(void);

void resume_timer(void);

void read_pot(void);

void start(void);

#endif
