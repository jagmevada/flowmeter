#pragma once
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>


// Extern declarations for shared objects
extern LiquidCrystal_I2C lcd;

extern float flowRate;
extern float totalVolume;
extern unsigned long totalTime;
extern bool useDummyData;
extern unsigned long dummyPulseInterval;

// Function prototypes
void updateDisplay();