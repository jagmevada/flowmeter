#include "disp.h"
#include "global.h"
#include "vessel.h"
#include <LiquidCrystal_I2C.h>

// LCD Configuration
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Shared variables (define here)


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

