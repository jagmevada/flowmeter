#include "disp.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "global.h"
#include "vessel.h"
#include "keypads.h" // For keypadState, KP_QUICK_ADJUST

// LCD address and size
#define LCD_ADDR 0x27
#define LCD_COLS 16
#define LCD_ROWS 2

LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

static DisplayPage currentPage = PAGE_MAIN;
static unsigned long lastUpdate = 0;
extern FlowSensor flow;
void initDisplay() {
    lcd.init();
    lcd.backlight();
    lcd.clear();
    updateDisplay();
}

void updateDisplay() {
    // Only update if enough time has passed or forced
    unsigned long now = millis();
    if (now - lastUpdate < DISP_UPDATE_INTERVAL && !blinkState) return;
    lastUpdate = now;
    static OperationMode previousMode = MODE_NORMAL;
    switch (currentMode) {
        case MODE_NORMAL:
            if(currentMode!=previousMode){  
                lcd.clear(); // Clear display on mode change                
            }
            if (inputMode) {
                showInputPage();
                currentPage = PAGE_INPUT;
            } else {
                showMainPage();
                currentPage = PAGE_MAIN;
            }
            previousMode = MODE_NORMAL;
            break;
        case MODE_CALIBRATION:
                  if(currentMode!=previousMode){  
                lcd.clear(); // Clear display on mode change
            }
            showCalibrationPage();
            currentPage = PAGE_CALIBRATION;
            previousMode = MODE_CALIBRATION;
            break;
    }
}

void forceDisplayRefresh() {
    lastUpdate = 0;
    updateDisplay();
}

void showMainPage() {
    // Row 1: F:xx.xLpm ON Vn (or V- if no vessel)

    lcd_flowrate_update();
    lcd_valvestate_update();
    lcd_vessel_update();


    // Row 2: T:  0/930L 00:00 
    lcd_currentvolume_update();
    lcd_timeupdate();

}

void showInputPage() {
    // Row 2: S:fff/xxxL  (set/total)
    // Clear entire second row first to avoid leftover characters from other pages
    lcd.setCursor(0, 1);
    lcd.print("                "); // 16 spaces
    // Start writing content
    lcd.setCursor(0, 1);
    lcd.print("S:");
    // Print dispensedVolume right-aligned into 3 chars at columns 2..4
    uint16_t dispVal = dispensedVolume > MAX_VOLUME ? MAX_VOLUME : dispensedVolume;
    char dBuf[4];
    snprintf(dBuf, sizeof(dBuf), "%3u", dispVal);
    lcd.setCursor(2, 1);
    lcd.print(dBuf);
    // Separator at column 5
    lcd.setCursor(5, 1);
    lcd.print("/");
    // Blinking logic for set volume entry
    static unsigned long lastBlink = 0;
    static bool blinkOn = true;
    unsigned long now = millis();
    if (inputMode) {
        if (now - lastBlink > 150) { // 150ms blink interval for faster blinking
            blinkOn = !blinkOn;
            lastBlink = now;
        }
    if (blinkOn) {
            // Show live input if editing, else setVolume
            char buf[4] = "   ";
            if (inputPos > 0) {
                int len = strlen(volumeInput);
                if (len > 3) len = 3;
                memcpy(buf + (3 - len), volumeInput, len);
                buf[3] = '\0';
            } else {
                buf[2] = '0'; buf[0] = buf[1] = ' ';
            }
            // Print set-volume right-aligned at cols 6..8
            lcd.setCursor(6,1);
            lcd.print(buf);
        } else {
            // Print spaces to blink out the entry
            lcd.setCursor(6,1);
            lcd.print("   ");
        }
    } else {
        uint16_t setVal = setVolume > 999 ? 999 : setVolume;
        char buf[4];
        snprintf(buf, sizeof(buf), "%3u", setVal);
        lcd.setCursor(6,1);
        lcd.print(buf);
    }
    lcd.print("L");
}

void showCalibrationPage() {

    // Placeholder for calibration info/page
    lcd.setCursor(0, 0);
    lcd.print("C");


}

void dispBlink() {
    blinkState = !blinkState;
    updateDisplay();
}

bool  lcdclearneeded() {
  static uint8_t prevDispLen = 0;
  static uint8_t prevSetLen = 0;

  uint8_t dispLen = String(dispensedVolume).length();
  uint8_t setLen  = String(setVolume).length();

  uint8_t totalLen     = dispLen + setLen + 2; // +2 for '/' and 'L'
  uint8_t prevTotalLen = prevDispLen + prevSetLen + 2;

 bool changed= (totalLen != prevTotalLen);
  // Update stored lengths
  prevDispLen = dispLen;
  prevSetLen  = setLen;
  return changed;
}

void lcd_valvestate_update(){
        lcd.setCursor(10,0);
        lcd.print(valveState ? "ON " : "OFF");
}

void lcd_vessel_update() {
    lcd.setCursor(14, 0);
    if ((currentVessel >= 0) && (currentVessel < 10)) {
        // lcd.print("V");
        lcd.print(round(vbat*10));
    } else {
        lcd.print("V-");
    }
}


void lcd_flowrate_update() {
    lcd.setCursor(0, 0);
    lcd.print("F:");
    if(flowRate<10){
      lcd.setCursor(2, 0);
      lcd.print("    "); // e.g. " 0.0", "12.3"
      lcd.setCursor(3, 0);
      lcd.print(flowRate, 1); // e.g. " 0.0", "12.3"
    }
    else{
      lcd.print(flowRate, 1); // e.g. " 0.0", "12.3"
    } 
    lcd.setCursor(6, 0);
    lcd.print("Lpm");
}

void lcd_currentvolume_update() {
lcd.setCursor(0, 1);
    lcd.print("T:"); // 2 spaces after T:
    if( clear2ndrow || lcdclearneeded() ){
        lcd.print("        ");
        clear2ndrow = false; // Reset flag after clearing
        }
    lcd.setCursor(2, 1);
        if(dispensedVolume>MAX_VOLUME){
        dispensedVolume = MAX_VOLUME; // Cap at MAX_VOLUME for display
    }
    lcd.print(dispensedVolume);
    lcd.print("/");
    if(setVolume>MAX_VOLUME){
        setVolume = MAX_VOLUME; // Cap at MAX_VOLUME for display
    }
    lcd.print(setVolume);
    lcd.print("L ");
}

void lcd_timeupdate(){
    lcd.setCursor(11,1);
    uint16_t sec = elapsedSeconds % 60;
    uint16_t min = elapsedSeconds / 60;
    if (min < 10) lcd.print("0");
    lcd.print(min); lcd.print(":");
    if (sec < 10) lcd.print("0");
    lcd.print(sec);
}

void caldataupdate(){
    timems =  flow.gettotaltime();
    count= flow.getPulse();   
    lcd.setCursor(0, 0);
    lcd.print("T:");
    lcd.print(timems, 1);
    lcd.setCursor(0, 1);
    lcd.print("P:");
    lcd.print(count);
    Serial.print("Time: ");
    Serial.print(timems);
    Serial.print("\tpulse: ");
    Serial.println(count);
    lcd.setCursor(8, 1);
    lcd.print("V:");
    dispensedVolumef += (float) flow.getVolume(); // of; // Update dispensed volume
    lcd.print(dispensedVolumef , 1);
    Serial.println(dispensedVolumef);
    lcd_valvestate_update();
}