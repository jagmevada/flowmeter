#include <Wire.h>
#include <global.h>



void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  
  // Initialize I2C LCD at 400kHz
  Wire.begin();
  Wire.setClock(400000);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  
  // Initialize pins
  pinMode(SOLENOID_VALVE_PIN, OUTPUT);
  valveoff; // Ensure valve is off at startup 
  
  // Initial display
  updateDisplay();
  
  // Record start time
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
    handleKeypress(); // Check for key presses
  // Generate dummy pulses if in test mode
  if (useDummyData && (millis() - lastDummyPulse > dummyPulseInterval)) {
    pulseCount++;
    lastDummyPulse = millis();
  }
  // if (useDummyData) simulateFlowProfile();
  
  // Detect current vessel
  detectVessel();
  
  // Update display periodically
  if (millis() - lastDisplayUpdate > displayUpdateInterval) {
    updateDisplay();
    lastDisplayUpdate = millis();
  }
}






// End of file
// This code is designed to run on an Arduino Nano with a YF-B5 flow sensor