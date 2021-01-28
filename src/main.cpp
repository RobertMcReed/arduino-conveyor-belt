#include <Arduino.h>
#include "motor.h"
#include "serial.h"

void setup() {
  Serial.begin(115200);

  motorSetup();
}

const uint8_t bufSize = 80;
char buf[bufSize];

void loop() {
  motorHandleLoop();

  if (readline(Serial.read(), buf, bufSize)) {
    int val = atoi(buf);

    if (!val) {
      if (!strcmp(buf, "O") || !strcmp(buf, "o") || !strcmp(buf, "0")) {
        Serial.println("REQUEST OFF");
        motorOff();
      } else if (!strcmp(buf, "F") || !strcmp(buf, "f")) {
        Serial.println("REQUEST FORWARDS");
        motorForwards();
      } else if (!strcmp(buf, "B") || !strcmp(buf, "b")) {
        Serial.println("REQUEST BACKWARDS");
        motorBackwards();
      } else if (!strcmp(buf, "p")) {
        Serial.println("Moving 30 steps");
        // motorMoveDistance(30l);
      } else {
        Serial.print("Unknown command: ");
        Serial.println(buf);
      }
    } else {
      Serial.print("REQUEST SPEED: ");
      Serial.println(val);
      motorSetSpeed(val);
    }
  }
}
