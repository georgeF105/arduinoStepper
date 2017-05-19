
// Control Stepper motors at the same time with different speeds
// and accelerations via serial commands

// Uses the the Adafruit Motor Shield V2 board
// http://learn.adafruit.com/adafruit-motor-shield-v2-for-arduino/overview

// Requires AccelStepper with AFMotor support (http://www.airspayce.com/mikem/arduino/AccelStepper/)
#include <AccelStepper.h>
// Requires the Adafruit Motor Shield V2 library (https://github.com/adafruit/Adafruit_Motor_Shield_V2_Library)
#include <Wire.h>
#include <Adafruit_MotorShield.h>
//#include "utility/Adafruit_PWMServoDriver.h"

#define NUM_STEPPERS 12
#define STEP_MODE DOUBLE // you can change this to SINGLE, DOUBLE or INTERLEAVE or MICROSTEP
#define motorSpeed 1200
#define motorAccel 1000
#define MAX_TRAVEL 30000 // Change this to set the max height of the steppers in steps

const char  eol = '\n'; // end of line character
const String stepperStart = " 1";
const String stepperStop = " 0";

String inputString = "";
//int inputArray[12];
int currentInputIndex = 0;
boolean positionChanged = false;


int inputArray[12] = {-MAX_TRAVEL, -MAX_TRAVEL, -MAX_TRAVEL, -MAX_TRAVEL, -MAX_TRAVEL, -MAX_TRAVEL, -MAX_TRAVEL, -MAX_TRAVEL, -MAX_TRAVEL, -MAX_TRAVEL, -MAX_TRAVEL, -MAX_TRAVEL};
int limitPins[NUM_STEPPERS] = {2, 3, 4, 5, 6, 8, 9, 10, 11, 12, 13, 14 };

boolean stepperRunning[NUM_STEPPERS] = {false, false, false, false, false, false,false, false, false, false, false, false};

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS0 = Adafruit_MotorShield(); // default board ID
Adafruit_MotorShield AFMS1 = Adafruit_MotorShield(0x61); // board ID + 1
Adafruit_MotorShield AFMS2 = Adafruit_MotorShield(0x62); // board ID + 2
Adafruit_MotorShield AFMS3 = Adafruit_MotorShield(0x63); // board ID + 3
Adafruit_MotorShield AFMS4 = Adafruit_MotorShield(0x64); // board ID + 4
Adafruit_MotorShield AFMS5 = Adafruit_MotorShield(0x65); // board ID + 5

// Connect a stepper motors with 200 steps per revolution (1.8 degree)
Adafruit_StepperMotor *motors[NUM_STEPPERS] = {
  AFMS0.getStepper(200, 1),
  AFMS0.getStepper(200, 2),
  AFMS1.getStepper(200, 1),
  AFMS1.getStepper(200, 2),
  AFMS2.getStepper(200, 1),
  AFMS2.getStepper(200, 2),
  AFMS3.getStepper(200, 1),
  AFMS3.getStepper(200, 2),
  AFMS4.getStepper(200, 1),
  AFMS4.getStepper(200, 2),
  AFMS5.getStepper(200, 1),
  AFMS5.getStepper(200, 2)
};

// wrappers for the Adafruit motors to work with AccelStepper
// first motor
void forwardstep0() { motors[0]->onestep(FORWARD, STEP_MODE); }
void backwardstep0() { motors[0]->onestep(BACKWARD, STEP_MODE); }
// second motor
void forwardstep1() { motors[1]->onestep(FORWARD, STEP_MODE); }
void backwardstep1() { motors[1]->onestep(BACKWARD, STEP_MODE); }
// third motor
void forwardstep2() { motors[2]->onestep(FORWARD, STEP_MODE); }
void backwardstep2() { motors[2]->onestep(BACKWARD, STEP_MODE); }
// forth motor
void forwardstep3() { motors[3]->onestep(FORWARD, STEP_MODE); }
void backwardstep3() { motors[3]->onestep(BACKWARD, STEP_MODE); }
// fith motor
void forwardstep4() { motors[4]->onestep(FORWARD, STEP_MODE); }
void backwardstep4() { motors[4]->onestep(BACKWARD, STEP_MODE); }
// sixth motor
void forwardstep5() { motors[5]->onestep(FORWARD, STEP_MODE); }
void backwardstep5() { motors[5]->onestep(BACKWARD, STEP_MODE); }
// seventh motor
void forwardstep6() { motors[6]->onestep(FORWARD, STEP_MODE); }
void backwardstep6() { motors[6]->onestep(BACKWARD, STEP_MODE); }
// eigth motor
void forwardstep7() { motors[7]->onestep(FORWARD, STEP_MODE); }
void backwardstep7() { motors[7]->onestep(BACKWARD, STEP_MODE); }
// ninth motor
void forwardstep8() { motors[8]->onestep(FORWARD, STEP_MODE); }
void backwardstep8() { motors[8]->onestep(BACKWARD, STEP_MODE); }
// tenth motor
void forwardstep9() { motors[9]->onestep(FORWARD, STEP_MODE); }
void backwardstep9() { motors[9]->onestep(BACKWARD, STEP_MODE); }
// eleventh motor
void forwardstep10() { motors[10]->onestep(FORWARD, STEP_MODE); }
void backwardstep10() { motors[10]->onestep(BACKWARD, STEP_MODE); }
// twelfth motor
void forwardstep11() { motors[11]->onestep(FORWARD, STEP_MODE); }
void backwardstep11() { motors[11]->onestep(BACKWARD, STEP_MODE); }

// wrap the motors in an AccelStepper object array
AccelStepper steppers[NUM_STEPPERS] = {
  AccelStepper(forwardstep0, backwardstep0),
  AccelStepper(forwardstep1, backwardstep1),
  AccelStepper(forwardstep2, backwardstep2),
  AccelStepper(forwardstep3, backwardstep3),
  AccelStepper(forwardstep2, backwardstep4),
  AccelStepper(forwardstep3, backwardstep5),
  AccelStepper(forwardstep0, backwardstep6),
  AccelStepper(forwardstep1, backwardstep7),
  AccelStepper(forwardstep2, backwardstep8),
  AccelStepper(forwardstep3, backwardstep9),
  AccelStepper(forwardstep2, backwardstep10),
  AccelStepper(forwardstep3, backwardstep11)
};

void setup() {  
    Serial.begin(57600);
    AFMS0.begin(); // start the Adafruit board
    AFMS1.begin(); // start the Adafruit board
    AFMS2.begin(); // start the Adafruit board
    AFMS3.begin(); // start the Adafruit board
    AFMS4.begin(); // start the Adafruit board
    AFMS5.begin(); // start the Adafruit board

    pinMode(limitPins[0],INPUT_PULLUP);
    pinMode(limitPins[1],INPUT_PULLUP);
    pinMode(limitPins[2],INPUT_PULLUP);
    pinMode(limitPins[3],INPUT_PULLUP);
    pinMode(limitPins[4],INPUT_PULLUP);
    pinMode(limitPins[5],INPUT_PULLUP);
    pinMode(limitPins[6],INPUT_PULLUP);
    pinMode(limitPins[7],INPUT_PULLUP);
    pinMode(limitPins[8],INPUT_PULLUP);
    pinMode(limitPins[9],INPUT_PULLUP);
    pinMode(limitPins[10],INPUT_PULLUP);
    pinMode(limitPins[11],INPUT_PULLUP);
    pinMode(limitPins[12],INPUT_PULLUP);
    updateMotorPositions();
}

void loop() {
  for( int i=0; i <  NUM_STEPPERS; i++) {
    steppers[i].run();
    // check to see if any moves have finished
    if ((steppers[i].distanceToGo() == 0 && stepperRunning[i]) || (digitalRead(limitPins[i]) == HIGH && stepperRunning[i])) {
      if (digitalRead(limitPins[i]) == HIGH) {
        steppers[i].setCurrentPosition(0);
        steppers[i].moveTo(0);
      }
      motors[i]->release();
      stepperRunning[i] = false;
      positionChanged = true;
    }
  }
  
  if (Serial.available() > 0) {
    serialInput();
  }

  if (positionChanged == true) {
    positionChanged = false;
    printMotorPositions();
  }
}

void serialInput() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if(inChar == 'P') {
      printMotorPositions();
    } else if(inChar == '?') {
      currentInputIndex = 0;
    } else if (inChar == ',') {
      inputArray[currentInputIndex] = inputString.toInt();
      inputString = "";
      currentInputIndex++;
    } else if (inChar == '\n') {
      updateMotorPositions();
    } else if (currentInputIndex > NUM_STEPPERS) {
      Serial.println('Error too many inputs');
    } else {
      inputString += inChar;      
    }
  }
}

void updateMotorPositions() {
  Serial.print("Updating motor positions ");
  for(int i = 0; i < NUM_STEPPERS; i++) {
    if (inputArray[i] > MAX_TRAVEL) {
      inputArray[i] = MAX_TRAVEL;
    }
    
    Serial.print(inputArray[i]);
    Serial.print(" ");
    stepperRunning[i] = true;
    steppers[i].setMaxSpeed(motorSpeed);
    steppers[i].setAcceleration(motorAccel);
    steppers[i].moveTo(inputArray[i]);
    
    inputArray[i] = 0;
  }
  Serial.println();  
}

void printMotorPositions() {
  Serial.print("X");
  for(int i = 0; i < NUM_STEPPERS; i++) {
    Serial.print(steppers[i].currentPosition());
    Serial.print(",");
  }
  Serial.println();
}

