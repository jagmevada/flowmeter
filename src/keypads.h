#pragma once
#include <Keypad.h>

// Keypad state enum for use in other modules
enum KeypadState { KP_IDLE, KP_INPUT, KP_QUICK_ADJUST };
extern KeypadState keypadState;
extern bool quickAdjustPending;
extern uint8_t quickAdjustTempStep;
extern Keypad keypad;
void handleKeypress();