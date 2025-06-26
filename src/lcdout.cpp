#include <Arduino.h>
#include "global.h"
#include "vessel.h"
#include "lcdout.h"
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2); // Use your LCD's I2C address and size

extern float flowRate;
extern bool valveState;
extern bool vessel_update;
extern unsigned long lastBlinkTime;
extern bool blinkState;
extern unsigned long totalTime;
extern float totalVolume;
extern unsigned char currentVessel;

void updateDisplay() {
  char buf[7];

  lcd.setCursor(0, 0);
  lcd.print("F:");
  dtostrf(flowRate, 4, 1, buf);
  lcd.print(buf);
  lcd.print("Lpm");
  lcd.setCursor(10, 0);
  if (valveState) {
    lcd.print("ON ");
  } else {
    lcd.print("OFF");
  }

  if (vessel_update || millis() - lastBlinkTime >= 1000) {
    lastBlinkTime = millis();
    blinkState = !blinkState;

    lcd.setCursor(14, 0);
    if (currentVessel > 9) {
      if (blinkState) {
        lcd.print("V-");
      } else {
        lcd.print("  ");
      }
    } else {
      lcd.print("V");
      lcd.print(currentVessel);
    }
    vessel_update = false;
  }

  lcd.setCursor(0, 1);
  lcd.print("T:");
  int liters = static_cast<int>(totalVolume + 0.5f);
  if (liters < 10) lcd.print("  ");
  else if (liters < 100) lcd.print(' ');
  lcd.print(liters);
  lcd.print('L');

  lcd.setCursor(10, 1);
  uint16_t minutes = totalTime / 60;
  uint8_t seconds = totalTime % 60;
  lcd.print("t");
  if (minutes < 10) lcd.print('0');
  lcd.print(minutes);
  lcd.print(':');
  if (seconds < 10) lcd.print('0');
  lcd.print(seconds);
}