

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

#define valveoff digitalWrite(SOLENOID_VALVE_PIN, LOW);
#define valveon  digitalWrite(SOLENOID_VALVE_PIN, HIGH);

typedef unsigned char u8;
typedef char i8;
typedef unsigned short  u16;
typedef unsigned long   u32;
typedef unsigned long long u64;
typedef float f32;
typedef short i16;
typedef int i32;
typedef long long i64;

// Pin Definitions
#define FLOW_SENSOR_PIN 3   // Interrupt-capable pin for flow sensor
#define SOLENOID_VALVE_PIN 2 // Control pin for solenoid valve
#define VESSEL_DETECT_PIN A0 // Analog pin for vessel detection

u8 key ;
// Keypad Configuration
const byte ROWS = 5;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'F','F','#','*'},
  {'1','2','3','^'},
  {'4','5','6','v'},
  {'7','8','9','E'},
  {'<','0','>','E'}
};

// Pin mapping (based on your D4–D12 soldered order)
byte colPins[4] = {4, 5, 6, 7};        // C1–C4 on D4–D7
byte rowPins[5] = {12, 11, 10, 9, 8};  // R1–R5 on D12–D8


// LCD Configuration
LiquidCrystal_I2C lcd(0x27, 16, 2); // Adjust I2C address if needed


Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);


// Display Update Control
u32 lastDisplayUpdate = 0;
const i32 displayUpdateInterval = 200; // ms between display updates

// Dummy Data Generation
bool useDummyData = true; // Set to false when using real sensor
u32 lastDummyPulse = 0;
u32 dummyPulseInterval = 1000; // ms between dummy pulses
bool valveState = false;


// Flow Sensor Constants (YF-B5)
const float calibrationFactor = 15; // Pulses per liter per minute
volatile uint16_t  pulseCount = 0;
float flowRate = 0.0;
float totalVolume = 0.0;
u32 totalTime = 0;
u32 oldTime = 0;
u8 currentVessel = 0; // Track the currently detected vessel
bool vessel_update = false; // Flag to indicate if vessel detection has changed
bool blinkState = true;
unsigned long lastBlinkTime = 0;
// Vessel Detection
const i16 vesselResistors[] = {0, 150, 300, 470, 680, 1000}; // Example resistor values in ohms
const i16 numVessels = sizeof(vesselResistors) / sizeof(vesselResistors[0]);

void handleKeypress();
void detectVessel();             // reads A0 and sets currentVessel
void updateDisplay();            // refreshes the 1602 LCD