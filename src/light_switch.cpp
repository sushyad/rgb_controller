#include "base.h"

#define SWITCH_PIN         5           // GPIO 5 - traditional light switch

unsigned long switchCount = 0;
bool prevSwitchState = false;
bool actionTaken = false;

void setupLightSwitch() {
  pinMode(SWITCH_PIN, INPUT_PULLUP);
}

void checkLightSwitch() {
  char scmnd[20];
  bool switchState = digitalRead(SWITCH_PIN);
  if (switchState != prevSwitchState) {
    prevSwitchState = switchState;
    switchCount = 0;
    actionTaken = false;
  } else {
    if (!actionTaken) {
      switchCount++;

      if (switchCount > 1) {
        actionTaken = true;
        snprintf_P(scmnd, sizeof(scmnd), commands[1]);
        do_cmnd(scmnd);              // Execute command internally 
      }
    }
  }
}
