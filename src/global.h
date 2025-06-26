#pragma once
#include <Arduino.h>
// Pin Definitions
#define FLOW_SENSOR_PIN 3   // Interrupt-capable pin for flow sensor
#define SOLENOID_VALVE_PIN 2 // Control pin for solenoid valve
#define VESSEL_DETECT_PIN A0 // Analog pin for vessel detection

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


extern f32 totalVolume;
extern u32 totalTime;
extern u32 oldTime;
extern bool vessel_update;
extern u8 currentVessel; // Only declare here, do not define
extern bool valveState; // State of the solenoid valve
extern bool blinkState; 
extern u32 lastBlinkTime; 
extern f32 flowRate;
extern bool useDummyData;
extern u32 dummyPulseInterval;