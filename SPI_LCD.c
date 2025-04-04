#include "SPI_LCD.h"
#include "DELAY.h"

// Initialize the LCD
void SPILCD_Init(ARM_DRIVER_SPI* spiDrv) {
	// Initilize SPI
	spiDrv->Initialize(NULL);

	// Enable SPI at full performance, and configure the SPI as Master,
	// using mode 3, bit order from MSB to LSB, disable default Slave
	// Select signal since an external SS signal is used, use 8 bit
	// data words and a transmission speed of 20 Mbit/sec
	spiDrv->PowerControl(ARM_POWER_FULL);
	spiDrv->Control(ARM_SPI_MODE_MASTER | ARM_SPI_CPOL1_CPHA1 | ARM_SPI_MSB_LSB | ARM_SPI_SS_MASTER_UNUSED | ARM_SPI_DATA_BITS(8), 20000000);

	// Configure additional control signals using GPIO
	// RESET: output signal used to reset the LCD
	// A0: output signal used to switch between send commands and data
	// CS: output signal chip select to enable the LCD controller chip
	GPIO_PortClock(1);
	GPIO_SetDir(LCD_PORT, LCD_RESET_PIN, GPIO_DIR_OUTPUT);
	GPIO_SetDir(LCD_PORT, LCD_A0_PIN, GPIO_DIR_OUTPUT);
	GPIO_SetDir(LCD_PORT, LCD_CS_PIN, GPIO_DIR_OUTPUT);
	PIN_Configure(LCD_PORT, LCD_RESET_PIN, PIN_FUNC_0, PIN_PINMODE_PULLUP, PIN_PINMODE_NORMAL);
	PIN_Configure(LCD_PORT, LCD_A0_PIN, PIN_FUNC_0, PIN_PINMODE_PULLUP, PIN_PINMODE_NORMAL);
	PIN_Configure(LCD_PORT, LCD_CS_PIN, PIN_FUNC_0, PIN_PINMODE_PULLUP, PIN_PINMODE_NORMAL);
}

// Send command cmd to the LCD
void SPILCD_SendCommand(ARM_DRIVER_SPI* spiDrv, uint8_t cmd) {
	ARM_SPI_STATUS status;

	// Set send-command mode and enable the LCD controller chip
	GPIO_PinWrite(LCD_PORT, LCD_A0_PIN, LCD_A0_COMMAND);
	GPIO_PinWrite(LCD_PORT, LCD_CS_PIN, LCD_CS_ACTIVE);

	// Send the 1-byte-command and wait for completion
	spiDrv->Send(&cmd, 1);
	do { status = spiDrv->GetStatus(); } while(status.busy);

	// Disable the LCD controller chip
	GPIO_PinWrite(LCD_PORT, LCD_CS_PIN, LCD_CS_INACTIVE);
}

// Send data to the LCD
void SPILCD_SendData(ARM_DRIVER_SPI* spiDrv, uint8_t* data, uint32_t size) {
	ARM_SPI_STATUS status;

	// Set send-data mode and enable the LCD controller chip
	GPIO_PinWrite(LCD_PORT, LCD_A0_PIN, LCD_A0_DATA);
	GPIO_PinWrite(LCD_PORT, LCD_CS_PIN, LCD_CS_ACTIVE);

	// Send the data and wait for completion
	spiDrv->Send(data, size);
	do { status = spiDrv->GetStatus(); } while(status.busy);

	// Disable the LCD controller chip
	GPIO_PinWrite(LCD_PORT, LCD_CS_PIN, LCD_CS_INACTIVE);
}

// Reset the LCD
void SPILCD_Reset(ARM_DRIVER_SPI* spiDrv) {
	// Set send-command mode and enable the LCD controller chip
	GPIO_PinWrite(LCD_PORT, LCD_CS_PIN, LCD_CS_ACTIVE);
	GPIO_PinWrite(LCD_PORT, LCD_A0_PIN, LCD_A0_COMMAND);
	
	// Activate the reset signal of the LCD for 50 microseconds,
	// and then deactivate it for 5 milliseconds.
	// (See LCD specifications)
	GPIO_PinWrite(LCD_PORT, LCD_RESET_PIN, LCD_RESET_ACTIVE);
	delay(50 * US);
	GPIO_PinWrite(LCD_PORT, LCD_RESET_PIN, LCD_RESET_INACTIVE);
	delay(5 * MS);

	// Disable the LCD controller chip
	GPIO_PinWrite(LCD_PORT, LCD_CS_PIN, LCD_CS_INACTIVE);
}

// Configure the LCD
void SPILCD_Configure(ARM_DRIVER_SPI* spiDrv) {
	// Check the LCD Datasheet for further information
	// File in Moodle: "Pantalla LCD NHD-C12832A1Z-FSW-FBW-3V3"
	// Page 7: Table of Commands
	// Note that commands are written as hex bytes here, but as binary
	// words in the document.
	SPILCD_SendCommand(spiDrv, 0xAE);
	SPILCD_SendCommand(spiDrv, 0xA2);
	SPILCD_SendCommand(spiDrv, 0xA0);
	SPILCD_SendCommand(spiDrv, 0xC8);
	SPILCD_SendCommand(spiDrv, 0x22);
	SPILCD_SendCommand(spiDrv, 0x2F);
	SPILCD_SendCommand(spiDrv, 0x40);
	SPILCD_SendCommand(spiDrv, 0xAF);
	SPILCD_SendCommand(spiDrv, 0x81);
	SPILCD_SendCommand(spiDrv, 0x17);
	SPILCD_SendCommand(spiDrv, 0xA6);
}

// Transfer the buffer to the LCD
void SPILCD_Transfer(ARM_DRIVER_SPI* spiDrv, uint8_t* buffer) {
	// Send buffer to the LCD page per page
	for(uint32_t page = 0; page < LCD_NUM_PAGES; page++) {
		// Set column low nibble 0 and high nibble 0
		SPILCD_SendCommand(spiDrv, 0x00);
		SPILCD_SendCommand(spiDrv, 0x10);
		
		// Set page address
		SPILCD_SendCommand(spiDrv, 0xB0 + page);
		
		// Send the page to the LCD
		SPILCD_SendData(spiDrv, &buffer[LCD_PAGE_SIZE * page], LCD_PAGE_SIZE);
	}
}
