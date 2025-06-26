#include <Wire.h>
<<<<<<< HEAD
#include <global.h>


=======
#include "global.h"
#include "hmi.h"
#include "vessel.h"

unsigned long lastDisplayUpdate = 0;
const long displayUpdateInterval = 200;
>>>>>>> temp-branch

void setup() {
  Serial.begin(9600);
  pinMode(SOLENOID_VALVE_PIN, OUTPUT);
  valveoff; // Ensure the valve is off at startup
  Wire.begin();
  Wire.setClock(400000);
  lcd.init();
  lcd.backlight();
  lcd.clear();
<<<<<<< HEAD
  
  // Initialize pins
  pinMode(SOLENOID_VALVE_PIN, OUTPUT);
  valveoff; // Ensure valve is off at startup 
  
  // Initial display
=======
>>>>>>> temp-branch
  updateDisplay();
  oldTime = millis();
}

/*************  ✨ Windsurf Command ⭐  *************/
/**
 * @brief Main program loop
 * @details
 * Scans the keypad for key presses, generates dummy pulses if in test mode,
 * processes flow measurements, detects the current vessel, and updates the
 * display periodically.
 */
void loop() {
<<<<<<< HEAD
    handleKeypress(); // Check for key presses
  // Generate dummy pulses if in test mode
  if (useDummyData && (millis() - lastDummyPulse > dummyPulseInterval)) {
    pulseCount++;
    lastDummyPulse = millis();
  }
  // if (useDummyData) simulateFlowProfile();
  
  // Detect current vessel
=======
    handleKeypress();
>>>>>>> temp-branch
  detectVessel();
  if (millis() - lastDisplayUpdate > displayUpdateInterval) {
    updateDisplay();
    lastDisplayUpdate = millis();
  }
}

<<<<<<< HEAD





// End of file
// This code is designed to run on an Arduino Nano with a YF-B5 flow sensor
=======
>>>>>>> temp-branch
