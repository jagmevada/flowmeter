#include "global.h"
#include <Keypad.h>
#include "vessel.h"
#include "keypads.h"

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

// State variables
enum KeypadState { KP_IDLE, KP_INPUT, KP_QUICK_ADJUST };
static KeypadState keypadState = KP_IDLE;

static uint8_t stepSize = 1; // Default step size
static bool quickAdjustPending = false;
static uint8_t quickAdjustTempStep = 0;

void handleKeypress() {
    char key = keypad.getKey();
    if (!key) return;
    DBG_PRINT("Key pressed: "); DBG_PRINTLN(key);

    // F1 toggles mode
    if (key == 'F') {
        currentMode = (currentMode == MODE_NORMAL) ? MODE_CALIBRATION : MODE_NORMAL;
        DBG_PRINT("Mode changed to: ");
        DBG_PRINTLN(currentMode == MODE_NORMAL ? "NORMAL" : "CALIBRATION");
        // Optionally: updateDisplay();
        return;
    }

    // Only normal mode implemented for now
    if (currentMode == MODE_NORMAL) {
        switch (keypadState) {
        case KP_IDLE:
            if (key == '*') {
                DBG_PRINTLN("Entering input mode");
                // Start or modify set volume
                inputMode = true;
                if (setVolume > 0) {
                    snprintf(volumeInput, sizeof(volumeInput), "%u", setVolume);
                    inputPos = strlen(volumeInput);
                } else {
                    volumeInput[0] = '\0';
                    inputPos = 0;
                }
                keypadState = KP_INPUT;
                // Optionally: updateDisplay();
            } else if (key == '#') {
                DBG_PRINTLN("Quick adjust mode start");
                quickAdjustPending = true;
                quickAdjustTempStep = 0;
                keypadState = KP_QUICK_ADJUST;
            } else if (key == '^') {
                setVolume += stepSize;
                DBG_PRINT("Set volume increased to: "); DBG_PRINTLN(setVolume);
                if (setVolume < dispensedVolume) {
                    setVolume = dispensedVolume;
                    valveoff();
                    valveState = false;
                    DBG_PRINTLN("Set volume < dispensed, valve closed");
                }
                // Optionally: updateDisplay();
            } else if (key == 'v') {
                if (setVolume > stepSize)
                    setVolume -= stepSize;
                else
                    setVolume = 0;
                DBG_PRINT("Set volume decreased to: "); DBG_PRINTLN(setVolume);
                if (setVolume < dispensedVolume) {
                    setVolume = dispensedVolume;
                    valveoff();
                    valveState = false;
                    DBG_PRINTLN("Set volume < dispensed, valve closed");
                }
                // Optionally: updateDisplay();
            } else if (key == 'S') {
                valveState = !valveState;
                DBG_PRINT("Manual valve toggle, state: "); DBG_PRINTLN(valveState ? "ON" : "OFF");
                if(valveState) valveon(); else valveoff();
            }
            break;

        case KP_INPUT:
            if (key >= '0' && key <= '9' && inputPos < 5) {
                volumeInput[inputPos++] = key;
                volumeInput[inputPos] = '\0';
                DBG_PRINT("Input buffer: "); DBG_PRINTLN(volumeInput);
                // Optionally: updateDisplay();
            } else if (key == '<' && inputPos > 0) {
                volumeInput[--inputPos] = '\0';
                DBG_PRINT("Input buffer (backspace): "); DBG_PRINTLN(volumeInput);
                // Optionally: updateDisplay();
            } else if (key == 'K') {
                setVolume = atoi(volumeInput);
                inputMode = false;
                keypadState = KP_IDLE;
                DBG_PRINT("Input confirmed, set volume: "); DBG_PRINTLN(setVolume);
                if (setVolume < dispensedVolume) {
                    setVolume = dispensedVolume;
                    valveoff();
                    valveState = false;
                    DBG_PRINTLN("Set volume < dispensed, valve closed");
                }
                // Optionally: updateDisplay();
            } else if (key == 'E') {
                inputMode = false;
                keypadState = KP_IDLE;
                DBG_PRINTLN("Input cancelled");
                // Optionally: updateDisplay();
            }
            break;

        case KP_QUICK_ADJUST:
            if (quickAdjustPending && key >= '0' && key <= '9') {
                quickAdjustTempStep = key - '0';
                DBG_PRINT("Quick adjust step set to: "); DBG_PRINTLN(quickAdjustTempStep == 0 ? 10 : quickAdjustTempStep);
                // Wait for up/down
            } else if (quickAdjustPending && (key == '^' || key == 'v')) {
                stepSize = (quickAdjustTempStep == 0) ? 10 : quickAdjustTempStep;
                DBG_PRINT("Quick adjust direction: "); DBG_PRINTLN(key == '^' ? "UP" : "DOWN");
                if (key == '^') {
                    setVolume += stepSize;
                } else {
                    if (setVolume > stepSize)
                        setVolume -= stepSize;
                    else
                        setVolume = 0;
                }
                DBG_PRINT("Set volume after quick adjust: "); DBG_PRINTLN(setVolume);
                if (setVolume < dispensedVolume) {
                    setVolume = dispensedVolume;
                    valveoff();
                    valveState = false;
                    DBG_PRINTLN("Set volume < dispensed, valve closed");
                }
                keypadState = KP_IDLE;
                quickAdjustPending = false;
                // Optionally: updateDisplay();
            } else if (key == 'E') {
                keypadState = KP_IDLE;
                quickAdjustPending = false;
                DBG_PRINTLN("Quick adjust cancelled");
                // Optionally: updateDisplay();
            }
            break;
        }
    } else if (currentMode == MODE_CALIBRATION) {
        // Calibration mode logic to be added later
        // For now, ignore all keys except 'F'
    }
}
