#include <Arduino.h>
#include "motor.h"
#include "serial.h"

void setup() {
  Serial.begin(115200);

  motorSetup();
}

const uint8_t bufSize = 20;
char buf[bufSize];

void loop() {
  motorHandleLoop();

  if (readline(Serial.read(), buf, bufSize)) {
    int val = atoi(buf);

    // send a debug message
    Serial.print("+++");
    Serial.println(buf);

    if (!val) {
      if (!strcmp(buf, "O") || !strcmp(buf, "o") || !strcmp(buf, "0")) {
        motorOff();
      } else if (!strcmp(buf, "F") || !strcmp(buf, "f")) {
        motorForwards();
      } else if (!strcmp(buf, "B") || !strcmp(buf, "b")) {
        motorBackwards();
      } else if (!strcmp(buf, "R") || !strcmp(buf, "r")) {
        motorReverse();
      } else {
        Serial.print("***UNKNOWN ");
        Serial.println(buf);
      }
    } else {
      bool isSpeed = false;
      for (int i = 0; i < bufSize; i++) {
        if (buf[i] == 0) break;
        else if (buf[i] == 's') {
          isSpeed = true;
          break;
        }
      }

      if (isSpeed) {
        motorSetNextSpeed(val);
      } else {
        motorMoveDistance(val);
      }
    }
  }
}
