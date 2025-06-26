#include "global.h"
f32 totalVolume = 0;
u32 totalTime = 0;
u32 oldTime = 0;
bool vessel_update = false;
bool valveState = false; // Initialize valve state to false (off)
bool blinkState = false; 
u32 lastBlinkTime = 0; // Initialize last blink time to 0
f32 flowRate = 0.0;
bool useDummyData = true;
u32 dummyPulseInterval = 1000;