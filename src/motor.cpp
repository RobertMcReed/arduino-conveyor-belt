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

float targetSteps = 0.0;
float targetDistance = 0.0;
const int numMicroSteps = 8; // 8 Micro Steps per Step (MS1 & MS2 high)
const int stepsPerRevolution = 200;
const int microStepsPerRevolution = numMicroSteps * stepsPerRevolution;
const float diameterOfRoller = 29.0; // measured diameter of 28.4, but observed value as indicated
const float distancePerRevolution = diameterOfRoller * 3.14159;

AccelStepper stepperLeft = AccelStepper(motorInterfaceType, stepPinLeft, dirPinLeft);
AccelStepper stepperRight = AccelStepper(motorInterfaceType, stepPinRight, dirPinRight);

enum {
  OFF,
  CLOCKWISE,
  ANTICLOCKWISE,
};

const int defaultMotorSpeed = 750;
int lastMotorSpeed = 750;
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

float distanceToMicroSteps(float distance) {
  distance = abs(distance);
  float numSteps = distance / distancePerRevolution * microStepsPerRevolution;

  return numSteps;
}

float microStepsToDistance(int steps) {
  steps = abs(steps);
  float distance = steps / microStepsPerRevolution * distancePerRevolution;

  return distance;
}

int motorDirection() {
  if (!motorSpeed) return 0;

  return motorSpeed > 0 ? 1 : -1;
}

void motorOff() {
  motorSpeed = 0;
  motorMode = OFF;
  targetSteps = 0;
  targetDistance = 0;
  Serial.println("***MOTOR_OFF");
}

void setMotor(bool requestBackwards) {
  motorMode = requestBackwards ? BACKWARDS : FORWARDS;

  if (!motorSpeed) {
    motorSpeed = lastMotorSpeed;
  }
  
  Serial.print("***MOTOR_SPEED ");
  Serial.println(motorSpeed);

  float newSpeed = abs(motorSpeed);

  int multiplier = MULTIPLIERS[(int)requestBackwards];
  newSpeed *= multiplier;
  motorSpeed = newSpeed;

  stepperLeft.setSpeed(newSpeed);
  stepperRight.setSpeed(newSpeed);
  targetSteps = 0;
  targetDistance = 0;
}

void motorForwards() {
  setMotor(forwards);
  Serial.println("***MOTOR_FORWARDS");
}

void motorBackwards() {
  setMotor(backwards);
  Serial.println("***MOTOR_BACKWARDS");
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

void motorSetNextSpeed(int speed) {
  bool motorIsOff = motorMode == OFF;
  motorSetSpeed(speed);
  if (motorIsOff) {
    motorMode = OFF;
  }
}

void motorReverse() {
  if (motorDirection() > 0) {
    motorBackwards();
  } else {
    motorForwards();
  }
}

void motorMoveDistance(int distance) {
  if (!distance) {
    motorOff();
    return;
  }

  if (targetDistance) {
    int stepsTaken = stepperLeft.currentPosition();
    float distanceTraveled = microStepsToDistance(stepsTaken);

    if (targetDistance < 0) {
      distanceTraveled *= -1;
    }

    Serial.print("***INTERRUPT ");
    Serial.println(distanceTraveled);
  }

  stepperLeft.setCurrentPosition(0);
  stepperRight.setCurrentPosition(0);

  int speed = motorSpeed;

  if (!speed) speed = lastMotorSpeed;
  if (!speed) speed = defaultMotorSpeed;

  speed = abs(speed);

  if (distance < 0) {
    speed *= -1;
  }

  Serial.print("***DISTANCE_SET ");
  Serial.println(distance);

  motorSetSpeed(speed);
  targetDistance = distance;
  targetSteps = distanceToMicroSteps(distance); // this must come after motorSetSpeed
}

void motorHandleLoop() {
  if (motorMode != OFF) {
    if (targetSteps) {
      long traveled = stepperLeft.currentPosition();
      traveled = abs(traveled);

      if (traveled >= targetSteps) {
        Serial.print("***DISTANCE_TRAVELED ");
        Serial.println(targetDistance);
        motorOff();
      }
    }

    stepperLeft.runSpeed();
    stepperRight.runSpeed();
  }
}
