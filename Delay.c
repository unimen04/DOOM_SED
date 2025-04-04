#include "Delay.h"

// Add a blocking delay in the code using Timer3
void delay(float delayInSec) {
	// Compute number of cycles
	// 25 MHz => 25Mticks/s * delayInSec = ticks/delay
	uint32_t prescaler = 1;
	uint32_t numTicks = (int32_t)((25000000 / prescaler) * delayInSec);

	// Enable timer3 setting PCONP[23]=1
	LPC_SC->PCONP |= (0x01 << 23);

	// Set prescaler and MR0 registers
	LPC_TIM3->PR = prescaler - 1;
	LPC_TIM3->MR0 = numTicks;

	// Reset all interrupts by setting all bits to '1'
	LPC_TIM3->IR |= 0x03F;

	// Disable interruptions, disable reset on TC=MR0, enable to stop
	// timer when TC=MR0 and set TCR[0] bit
	// set bit 2, reset bits 1 and 0
	LPC_TIM3->MCR = (LPC_TIM3->MCR | 0x004) & 0xFFC;

	// Enable timer and reset TC and PR synchronously
	// set bits 1 and 0 in TCR
	LPC_TIM3->TCR |= 0x03;

	// Disable reset on TC and PR
	// reset bit 1 in TCR
	LPC_TIM3->TCR &= 0x0D;

	// Wait until delay time has elapsed, i.e., timer will be stopped
	// and TCR[0] will be set to '0'.
	while(LPC_TIM3->TCR & 0x01);
}
