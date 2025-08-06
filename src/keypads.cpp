#include "keypads.h"
#include "global.h"
#include <Keypad.h>
#include "vessel.h"
#include "disp.h"
// Debug macro
#define DEBUG 1
#if DEBUG
  #define DBG_PRINTLN(x) Serial.println(x)
  #define DBG_PRINT(x) Serial.print(x)
#else
  #define DBG_PRINTLN(x)
  #define DBG_PRINT(x)
#endif

// Keypad Configuration
const byte ROWS = 5;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'F','S','#','*'},
  {'1','2','3','^'},
  {'4','5','6','v'},
  {'7','8','9','E'},
  {'<','0','>','K'}
};

byte colPins[4] = {4, 5, 6, 7};
byte rowPins[5] = {12, 11, 10, 9, 8};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void handleKeypress() {
    char key = keypad.getKey();
    if (!key) return;
    DBG_PRINT("Key pressed: "); DBG_PRINTLN(key);

    // F1 toggles mode
    if (key == 'F') {
        if(!setmode){
        currentMode = (currentMode == MODE_NORMAL) ? MODE_CALIBRATION : MODE_NORMAL;
        DBG_PRINT("Mode changed to: ");
        DBG_PRINTLN(currentMode == MODE_NORMAL ? "NORMAL" : "CALIBRATION");
        // Optionally: updateDisplay();
        }
        return;
    }

    // Only normal mode implemented for now
    if (currentMode == MODE_NORMAL) {
        switch (keypadState) {
        case KP_IDLE:
            if (key == '*') {
                setmode = true;                 
                DBG_PRINTLN("Entering input mode");
                inputMode = true;
                if (setVolume > 0) {
                    snprintf(volumeInput, sizeof(volumeInput), "%u", setVolume);
                    inputPos = strlen(volumeInput);
                } else {
                    volumeInput[0] = '\0';
                    inputPos = 0;
                }
                keypadState = KP_INPUT;
                displayUpdateInterval = 100; // Faster display update in input mode
                // Optionally: updateDisplay();
            } else if (key == '<') {
                if(setVolume > 1) {
                setVolume--;
                DBG_PRINT("Reduce set volume: ");
                DBG_PRINTLN(setVolume);
                lcd_currentvolume_update(); // Update display immediately
                }
                // Optionally: updateDisplay();
             } else if (key == '>') {
                if (setVolume < MAX_VOLUME) {
                    setVolume++;
                    DBG_PRINT("Increase set volume: ");
                    DBG_PRINTLN(setVolume);
                    lcd_currentvolume_update(); // Update display immediately
                }
                    // Optionally: updateDisplay();
            } else if (key == '^') {
                if(setVolume < ((u16) MAX_VOLUME - (u16)stepSize)) {
                    setVolume += stepSize;
                    DBG_PRINT("Set volume increased to: ");
                    DBG_PRINTLN(setVolume);
                    lcd_currentvolume_update(); // Update display immediately
                }
                // Optionally: updateDisplay();
            } else if (key == 'v') {
                if (setVolume >= stepSize) {
                    setVolume -= stepSize;
                } else {
                    setVolume = 0;
                }
                lcd_currentvolume_update(); // Update display immediately
                DBG_PRINT("Set volume decreased to: ");
                    DBG_PRINTLN(setVolume);
                if (setVolume < dispensedVolume) {
                    // setVolume = dispensedVolume;
                    valveoff();
                    valveState = false;
                    lcd_valvestate_update();
                    DBG_PRINTLN("Set volume < dispensed, valve closed");
                }
                // Optionally: updateDisplay();
            } else if (key == 'S') {
                
                valveState = !valveState;
                if(valveState){
                      if(setVolume > dispensedVolume) {
                        valveon();
                        flow.read();
                        DBG_PRINTLN("Valve manual on");
                    } else {
                        DBG_PRINTLN("Set volume too low, valve cannot be turned on");
                        valveState = false; // Reset valve state if it cannot be turned on
                    }
                }
                else{
                    DBG_PRINTLN("Valve manual off");
                    valveoff(); // Turn off the valve if toggled off
                }            
                lcd_valvestate_update(); // Update valve state on display
           
            } else if (key == '#') {
                // Reset timer
                elapsedSeconds = 0;
                lcd_timeupdate(); // Update display immediately
                DBG_PRINTLN("Timer reset to 0 by # key");
                // Optionally: updateDisplay();
            }
            break;

        case KP_INPUT:

            if (key >= '0' && key <= '9' && inputPos < 3) {
                DBG_PRINT("setVolume: ");
                DBG_PRINTLN(setVolume);
                    // Try appending the digit and check result
                    volumeInput[inputPos] = key;
                    volumeInput[inputPos + 1] = '\0';

                    int tempVolume = atoi(volumeInput);
                    if (tempVolume <= MAX_VOLUME) {
                        inputPos++;
                        DBG_PRINT("Input buffer: "); DBG_PRINTLN(volumeInput);
                        // Optionally: updateDisplay();
                    } else {
                        // Revert the character if it exceeds limit
                        volumeInput[inputPos] = '\0';
                        DBG_PRINTLN("Ignored: volume exceeds 999");
                    }

                } else if (key == '<' && inputPos > 0) {
                    // Handle backspace
                    volumeInput[--inputPos] = '\0';
                    DBG_PRINT("Input buffer (backspace): "); DBG_PRINTLN(volumeInput);
                    // Optionally: updateDisplay();

                } else if (key == 'K') {
                    // Confirm input
                    setVolume = atoi(volumeInput);
                    inputMode = false;
                    keypadState = KP_IDLE;
                    displayUpdateInterval = 200; // Restore normal update interval
                    DBG_PRINT("Input confirmed, set volume: "); DBG_PRINTLN(setVolume);

                    if (setVolume < dispensedVolume) {
                        // setVolume = dispensedVolume;
                        valveoff();
                        valveState = false;
                        lcd_valvestate_update();
                        DBG_PRINTLN("Set volume < dispensed, valve closed");
                    }
                    setmode = false; // Reset set mode flag
                    // Optionally: updateDisplay();

                } else if (key == 'E') {
                    // Cancel input
                    inputMode = false;
                    keypadState = KP_IDLE;
                    displayUpdateInterval = 200; // Restore normal update interval
                    DBG_PRINTLN("Input cancelled");
                    setmode = false; // Reset set mode flag
                    // Optionally: updateDisplay();
                } else if (key == '#') {
                    // Complete reset in input mode
                    elapsedSeconds = 0;
                    setVolume = 0;
                    dispensedVolume = 0;
                    dispensedVolumef = 0.0; // Reset float volume
                    volumeInput[0] = '\0';
                    inputPos = 0;
                    inputMode = false;
                    keypadState = KP_IDLE;
                    valveoff(); // Ensure valve is off
                    valveState = false; // Reset valve state
                    
                    clear2ndrow = true; // Clear second row on reset                   
                    displayUpdateInterval = 200; // Restore normal display update interval
                    DBG_PRINTLN("Complete reset: timer, setVolume, dispensedVolume set to 0 by # in input mode");
                    setmode = false;
                    flow.read();
                    flow.resetPulse(); // Reset flow sensor pulse count
                    flow.resetVolume(); // Reset flow sensor volume
                }
            break;
default:
            DBG_PRINTLN("Unknown key in input mode");
            break;

        }
    } else if (currentMode == MODE_CALIBRATION) {
        if (key == 'S') {
                DBG_PRINTLN("S prssed");
                valveState = !valveState;
                if(valveState){
                    lcd.clear();
                    flow.read();
                    flow.resetPulse();
                    flow.resetVolume();
                    flow.resettime();
                    flow.getVolume();
                    valveon();
                    flow.read();
                    dispensedVolumef=0;
                    caldataupdate(); 
                    DBG_PRINTLN("valve ON");
                }else{
                    flow.read();
                    count = flow.getPulse();
                    timems = flow.gettotaltime();
                    valveoff(); 
                    caldataupdate(); 
                    DBG_PRINTLN("valve OFF");
                }
                lcd_valvestate_update(); // Update valve state on display
            }else{
                // other key input 
            }
    }else{
// other modes
    }
}


