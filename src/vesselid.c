#include <global.h>
// Detect vessel based on analog input
/**
 * @brief Detects the current vessel by reading the value from the analog pin.
 * 
 * This function uses predefined thresholds based on resistor values to map 
 * the analog input to a specific vessel index. If the detected vessel changes,
 * it resets the total volume and time.
 */
void detectVessel() {
  u16 sensorValue = analogRead(VESSEL_DETECT_PIN);
  u8 detectedVessel = 0;  // Default: no vessel detected

  // Map sensor value to vessel index based on predefined resistor values
  if(sensorValue > 100){
  if (sensorValue < 132)        // R = 1.2kΩ, ADC ≈ 110
    detectedVessel = 0;
  else if (sensorValue < 183)   // R = 1.8kΩ, ADC ≈ 155
    detectedVessel = 1;
  else if (sensorValue < 238)   // R = 2.7kΩ, ADC ≈ 210
    detectedVessel = 2;
  else if (sensorValue < 316)   // R = 3.9kΩ, ADC ≈ 266
    detectedVessel = 3;
  else if (sensorValue < 416)   // R = 5.6kΩ, ADC ≈ 366
    detectedVessel = 4;
  else if (sensorValue < 510)   // R = 8.2kΩ, ADC ≈ 462
    detectedVessel = 5;
  else if (sensorValue < 600)   // R = 12kΩ, ADC ≈ 547
    detectedVessel = 6;
  else if (sensorValue < 660)   // R = 18kΩ, ADC ≈ 635
    detectedVessel = 7;
  else if (sensorValue < 730)   // R = 27kΩ, ADC ≈ 688
    detectedVessel = 8;
  else if (sensorValue < 850)   // R = 39kΩ, ADC ≈ 767
    detectedVessel = 9;
  else                          // ADC > 850 → no resistor connected (open)
    detectedVessel = 10;        // No vessel detected
  } else {
    detectedVessel = 20;        // No vessel detected
  }
    // Check if the detected vessel has changed
  if (detectedVessel != currentVessel) {
    currentVessel = detectedVessel;
    totalVolume = 0;  // Reset total volume when vessel changes
    totalTime = 0;    // Reset total time when vessel changes
    vessel_update = true; // Set flag to indicate vessel change
  }
}