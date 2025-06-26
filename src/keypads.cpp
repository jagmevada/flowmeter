#include <Keypad.h>
#include "global.h"
#include "vessel.h"
#include "keypads.h"

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
  if (!key) return; // No key pressed
  switch (key) {
    case 'E': // ESC key
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
    case 'S': // Function key
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
