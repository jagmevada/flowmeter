#include "global.h"
#include "keyin.h"
#include <Keypad.h>

extern Keypad keypad;
extern bool useDummyData;
extern unsigned long dummyPulseInterval;
extern bool valveState;
extern float totalVolume;
extern unsigned long totalTime;

void handleKeypress() {
  char key = keypad.getKey();
  if (!key) return;
  switch (key) {
    case 'E':
      valveState = LOW;
      valveoff;
      totalVolume = 0;
      totalTime = 0;
      break;
    case '^':
      if (useDummyData) dummyPulseInterval = max(100, dummyPulseInterval - 100);
      break;
    case 'v':
      if (useDummyData) dummyPulseInterval += 100;
      break;
    case 'S':
      valveState = !valveState;
      if (valveState) {
        valveon;
      } else {
        valveoff;
      }
      break;
    // Add other key handlers as needed
  }
}