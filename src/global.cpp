#include "global.h"
f32 totalVolume = 0;
u32 totalTime = 0;
u32 oldTime = 0;
bool vessel_update = false;
bool valveState = false; // Initialize valve state to false (off)
bool prevvalveState = false; // Previous valve state for change detection
bool blinkState = false; 
u32 lastBlinkTime = 0; // Initialize last blink time to 0
f32 flowRate = 0.0;
bool useDummyData = true;
u32 dummyPulseInterval = 1000;

uint16_t setVolume = 0;
uint16_t dispensedVolume = 0;
float dispensedVolumef= 0.0; // Use float for calculations, but display as uint16_t
bool inputMode = false;
char volumeInput[4] = "";
uint8_t inputPos = 0;
OperationMode currentMode = MODE_NORMAL;

uint16_t elapsedSeconds = 0;
uint8_t stepSize = 10;

KeypadState keypadState = KP_IDLE;
bool quickAdjustPending = false;
uint8_t quickAdjustTempStep = 0;
bool clear2ndrow = false; // Flag to indicate if the second row needs clearing
u32 displayUpdateInterval = 200; // Global variable for display update interval
bool setmode = false;
volatile double timems = 0.0;
volatile u32 count =0;
// Battery monitoring defaults
float vbat = 3.6; // measured single-cell voltage
bool VBATOK = true; // default HIGH (OK)