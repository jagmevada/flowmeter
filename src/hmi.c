#include <global.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
// Handle keypad input
void handleKeypress() {
  key = keypad.getKey();
  if (!key) return; // No key pressed
  switch (key) {
    case 'E': // ESC key
      // Stop flow and reset counters
      valveState = LOW;
      valveoff;
      totalVolume = 0;
      totalTime = 0;
      break;
    case '^': // Up arrow - increase flow (simulated)
      if (useDummyData) dummyPulseInterval = max(100, dummyPulseInterval - 100);
      break;
    case 'v': // Down arrow - decrease flow (simulated)
      if (useDummyData)
      dummyPulseInterval += 100;
      break;
    case 'F': // Function key
      // Toggle solenoid valve
      valveState = !valveState;
      if(valveState){
        valveon;
      }else{
        valveoff; 
      }
      break;
    // Add other key handlers as needed
  }
}




// ---- Update LCD display (16×2) ----
/**
 * @brief Update the 16x2 LCD display with the current flow rate, total volume,
 *        and elapsed time.
 * @details
 * The display is divided into two rows. The first row shows the flow rate in
 * liters per minute, the valve state, and the detected vessel number. The
 * second row shows the total volume in liters and the elapsed time in minutes
 * and seconds.
 */
void updateDisplay() {
  char buf[7];

  /* -------- first row -------- */
  lcd.setCursor(0, 0);          // F:xx.xL/min
  lcd.print("F:");             // Flow rate label
  dtostrf(flowRate, 4, 1, buf); // width 4, 1 decimal  (-0.0 … 30.0)
  lcd.print(buf);              // Print the flow rate value
  lcd.print("Lpm");             // Liters per minute unit
  lcd.setCursor(10, 0);      
  if( valveState ) {
    lcd.print("ON ");           // Valve state label
  } else {
    lcd.print("OFF");           // Valve state label
  }

if(vessel_update || millis() - lastBlinkTime >= 1000){ // update every second
    lastBlinkTime = millis();
    blinkState = !blinkState;
    
    lcd.setCursor(14, 0);         // ………V:y   (right-most)
          // Vessel label
    if(currentVessel > 9) {  // no valid vessel detected
          if(blinkState) {
              lcd.print("V-");    // blink "V"
          } else {
              lcd.print("  ");    // clear the space to create blinking effect
          }
    }
    else {
          lcd.print("V");         // valid vessel detected
      lcd.print(currentVessel);        
  }
    vessel_update = false;        // Reset flag after updating display
  }


  /* -------- second row -------- */
  lcd.setCursor(0, 1);          // T:xxxL
  lcd.print("T:");             // Total volume label
  int liters = static_cast<int>(totalVolume + 0.5f);   // round to whole litres
  if      (liters < 10)   lcd.print("  ");  // 2 leading spaces
  else if (liters < 100)  lcd.print(' ');   // 1 leading space
  lcd.print(liters);                     // 0-999
  lcd.print('L');                       // Liters unit

  lcd.setCursor(10, 1);          // ……t:mm:ss (right-aligned)
  uint16_t minutes = totalTime / 60;
  uint8_t  seconds = totalTime % 60;
  lcd.print("t");              // Time label
  if (minutes < 10) lcd.print('0');
  lcd.print(minutes);          // minutes
  lcd.print(':');              // separator
  if (seconds < 10) lcd.print('0');
  lcd.print(seconds);          // seconds
}

