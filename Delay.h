#ifndef DELAY_H_
#define DELAY_H_

#include <LPC17xx.h>

// Factors to convert milliseconds or microseconds to seconds.
// Examples:
//   15 seconds                        : delay(15.0);
//   15 milliseconds (0.015    seconds): delay(15.0 * MS);
//   15 microseconds (0.000015 seconds): delay(15.0 * US);
#define MS 1.e-3
#define US 1.e-6

// Add a blocking delay in the code using Timer3
void delay(float delayInSec);

#endif /* DELAY_H_ */
