#include <Arduino.h>
#include "global.h"
#include "vessel.h"

u8 currentVessel = 0; // Only define here

void detectVessel() {
  u16 sensorValue =102;// analogRead(VESSEL_DETECT_PIN);
  u8 detectedVessel = 0;  // Default: no vessel detected

  // Map sensor value to vessel index based on predefined resistor values
  if(sensorValue > 100){
    if (sensorValue < 132)
      detectedVessel = 0;
    else if (sensorValue < 183)
      detectedVessel = 1;
    else if (sensorValue < 238)
      detectedVessel = 2;
    else if (sensorValue < 316)
      detectedVessel = 3;
    else if (sensorValue < 416)
      detectedVessel = 4;
    else if (sensorValue < 510)
      detectedVessel = 5;
    else if (sensorValue < 600)
      detectedVessel = 6;
    else if (sensorValue < 660)
      detectedVessel = 7;
    else if (sensorValue < 730)
      detectedVessel = 8;
    else if (sensorValue < 850)
      detectedVessel = 9;
    else
      detectedVessel = 10;
  } else {
    detectedVessel = 20;
  }
  // Check if the detected vessel has changed
  if (detectedVessel != currentVessel) {
    currentVessel = detectedVessel;
    totalVolume = 0;
    totalTime = 0;
    vessel_update = true;
    Serial.print("Detected vessel: ");
    Serial.println(currentVessel);
  }
}