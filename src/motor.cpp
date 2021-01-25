#include <Arduino.h>

#include <AccelStepper.h>
#include <MultiStepper.h>

// Define stepper motor connections and steps per revolution:
#define dirPinLeft D5
#define stepPinLeft D6
#define dirPinRight D1
#define stepPinRight D2
#define motorInterfaceType 1

const int MAX_SPEED = 1000;
const bool backwards = true;
const bool forwards = false;

AccelStepper stepperLeft = AccelStepper(motorInterfaceType, stepPinLeft, dirPinLeft);
AccelStepper stepperRight = AccelStepper(motorInterfaceType, stepPinRight, dirPinRight);

enum {
  OFF,
  CLOCKWISE,
  ANTICLOCKWISE,
};

int lastMotorSpeed = 750; // default to 500
int motorSpeed = 0;

const int FORWARDS = CLOCKWISE;
const int BACKWARDS = ANTICLOCKWISE;

const int FORWARDS_MULTIPLIER = FORWARDS == CLOCKWISE ? -1 : 1;
const int BACKWARDS_MULTIPLIER = BACKWARDS == ANTICLOCKWISE ? 1 : -1;

const int MULTIPLIERS[2] = { FORWARDS_MULTIPLIER, BACKWARDS_MULTIPLIER };

int motorMode = OFF;

void motorSetup() {
  stepperLeft.setMaxSpeed(MAX_SPEED);
  stepperRight.setMaxSpeed(MAX_SPEED);
  stepperLeft.setAcceleration(100);
  stepperRight.setAcceleration(100);
  stepperLeft.setSpeed(motorSpeed);
  stepperRight.setSpeed(motorSpeed);
}

int motorDirection() {
  if (!motorSpeed) return 0;

  return motorSpeed > 0 ? 1 : -1;
}

void motorOff() {
  motorSpeed = 0;
  motorMode = OFF;
}

void setMotor(bool requestBackwards) {
  motorMode = requestBackwards ? BACKWARDS : FORWARDS;

  if (!motorSpeed) {
    motorSpeed = lastMotorSpeed;
  }
  
  float newSpeed = abs(motorSpeed);

  int multiplier = MULTIPLIERS[(int)requestBackwards];
  newSpeed *= multiplier;
  motorSpeed = newSpeed;

  stepperLeft.setSpeed(newSpeed);
  stepperRight.setSpeed(newSpeed);
}

void motorForwards() {
  setMotor(forwards);
}

void motorBackwards() {
  setMotor(backwards);
}

void motorSetSpeed(int speed) {
  if (abs(speed) > MAX_SPEED) {
    if (speed > 0)  speed = MAX_SPEED;
    else speed = -MAX_SPEED;
  }

  motorSpeed = speed;

  if (speed) {
    lastMotorSpeed = speed;
  }

  if (motorSpeed > 0) {
    motorForwards();
  } else if (motorSpeed) {
    motorBackwards();
  } else {
    motorOff();
  }
}

void motorReverse() {
  if (motorDirection() > 0) {
    Serial.println("Setting motor backwards");
    motorBackwards();
  } else {
    Serial.println("Setting motor forwards");
    motorForwards();
  }
}

void motorHandleLoop() {
  if (motorMode != OFF) {
    stepperLeft.runSpeed();
    stepperRight.runSpeed();
  }
}
