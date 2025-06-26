#include <global.h>
#include <LiquidCrystal_I2C.h>


void detectVessel();             // reads A0 and sets currentVessel
void updateDisplay();            // refreshes the 1602 LCD
void handleKeypress();   // processes one keypad key
