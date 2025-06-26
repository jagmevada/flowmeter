#ifndef GLOBAL_H
#define GLOBAL_H

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

extern u8 key;
// Keypad Configuration
extern const byte ROWS;
extern const byte COLS;
extern char keys[ROWS][COLS];
extern byte colPins[4];
extern byte rowPins[5];

// LCD Configuration
extern LiquidCrystal_I2C lcd;

extern Keypad keypad;

// Display Update Control
extern u32 lastDisplayUpdate;
extern const i32 displayUpdateInterval;

// Dummy Data Generation
extern bool useDummyData; // Set to false when using real sensor
extern u32 lastDummyPulse;
extern u32 dummyPulseInterval; // ms between dummy pulses
extern bool valveState;

// Flow Sensor Constants (YF-B5)
extern const float calibrationFactor; // Pulses per liter per minute
extern volatile uint16_t  pulseCount;
extern float flowRate;
extern float totalVolume;
extern u32 totalTime;
extern u32 oldTime;
extern u8 currentVessel; // Track the currently detected vessel
extern bool vessel_update; // Flag to indicate if vessel detection has changed
extern bool blinkState;
extern unsigned long lastBlinkTime;
// Vessel Detection
extern const i16 vesselResistors[]; // Example resistor values in ohms
extern const i16 numVessels;

#ifdef __cplusplus
extern "C" {
#endif
void handleKeypress();
void detectVessel();             // reads A0 and sets currentVessel
void updateDisplay();            // refreshes the 1602 LCD
#ifdef __cplusplus
}
#endif

#endif // GLOBAL_H
