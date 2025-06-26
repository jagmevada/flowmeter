#include <Wire.h>
#include "global.h"
#include "disp.h"
#include "vessel.h"
#include "keypads.h"

unsigned long lastDisplayUpdate = 0;
const long displayUpdateInterval = 200;

void setup() {
  Serial.begin(9600);
  pinMode(SOLENOID_VALVE_PIN, OUTPUT);
  valveoff; // Ensure the valve is off at startup
  Wire.begin();
  Wire.setClock(400000);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  updateDisplay();
  oldTime = millis();
}

void loop() {
    handleKeypress();
    detectVessel();
    if (millis() - lastDisplayUpdate > displayUpdateInterval) {
    updateDisplay();
    lastDisplayUpdate = millis();
  }
}

