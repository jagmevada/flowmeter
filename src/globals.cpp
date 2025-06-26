#include "global.h"

u8 key;

const byte ROWS = 5;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'F','F','#','*'},
  {'1','2','3','^'},
  {'4','5','6','v'},
  {'7','8','9','E'},
  {'<','0','>','E'}
};

byte colPins[4] = {4, 5, 6, 7};
byte rowPins[5] = {12, 11, 10, 9, 8};

LiquidCrystal_I2C lcd(0x27, 16, 2);
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

u32 lastDisplayUpdate = 0;
const i32 displayUpdateInterval = 200;

bool useDummyData = true;
u32 lastDummyPulse = 0;
u32 dummyPulseInterval = 1000;
bool valveState = false;

const float calibrationFactor = 15;
volatile uint16_t pulseCount = 0;
float flowRate = 0.0;
float totalVolume = 0.0;
u32 totalTime = 0;
u32 oldTime = 0;
u8 currentVessel = 0;
bool vessel_update = false;
bool blinkState = true;
unsigned long lastBlinkTime = 0;

const i16 vesselResistors[] = {0, 150, 300, 470, 680, 1000};
const i16 numVessels = sizeof(vesselResistors) / sizeof(vesselResistors[0]);

