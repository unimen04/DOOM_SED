#ifndef SPI_LCD_H_
#define SPI_LCD_H_

#include <Driver_SPI.h>
#include <PIN_LPC17xx.h>
#include <GPIO_LPC17xx.h>

#define LCD_PORT           0
#define LCD_RESET_PIN      8 // Reset LCD (active when low)
#define LCD_A0_PIN         6 // Sending command or data?
#define LCD_CS_PIN        18 // Chip Select LCD (active when low)

#define LCD_RESET_ACTIVE   0 // Reset active=low
#define LCD_RESET_INACTIVE 1 // Reset inactive=high

#define LCD_CS_ACTIVE      0 // Chip Select active=low
#define LCD_CS_INACTIVE    1 // Chip Select inactive=high

#define LCD_A0_COMMAND     0 // Send command
#define LCD_A0_DATA        1 // Send data

// Constants defining LCD configurations such as LCD size (columns and
// rows), rows per page, page size and memory size.
#define LCD_NUM_COLS          128
#define LCD_NUM_PAGES         4
#define LCD_NUM_ROWS_PER_PAGE 8
#define LCD_NUM_ROWS          (LCD_NUM_PAGES * LCD_NUM_ROWS_PER_PAGE)
#define LCD_PAGE_SIZE         LCD_NUM_COLS   // each column requires 1 byte
#define LCD_MEM_SIZE          (LCD_NUM_COLS * LCD_NUM_PAGES)

// Initialize the LCD
void SPILCD_Init(ARM_DRIVER_SPI* spiDrv);

// Send command cmd to the LCD
void SPILCD_SendCommand(ARM_DRIVER_SPI* spiDrv, uint8_t cmd);

// Send data to the LCD
void SPILCD_SendData(ARM_DRIVER_SPI* spiDrv, uint8_t* data, uint32_t size);

// Reset the LCD
void SPILCD_Reset(ARM_DRIVER_SPI* spiDrv);

// Configure the LCD
void SPILCD_Configure(ARM_DRIVER_SPI* spiDrv);

// Transfer the buffer to the LCD
void SPILCD_Transfer(ARM_DRIVER_SPI* spiDrv, uint8_t* buffer);

#endif /* SPI_LCD_H_ */
