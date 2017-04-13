//Including all the required libraries here. Wire.h required for I2C intercommunication
#include <AccelStepper.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

//Creating the Adafruit motor shield object. Not sure this is needed with the AccelStepper library
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

Adafruit_MotorShield AFMSbot(0x60); //this is the default address for I2C communication
Adafruit_MotorShield AFMSmid(0x61); //this is the for the middle shield
Adafruit_MotorShield AFMStop(0x62); //this is for the top shiled

//Connecting the three steppers for backstops here. Motor is 200 steps/rev 
//* here is for declaring pointer for an instance of the class
Adafruit_StepperMotor *myStepper3 = AFMSmid.getStepper(200, 1); //stepper on backstop 1
Adafruit_StepperMotor *myStepper4 = AFMSmid.getStepper(200, 2); //stepper on backstop 2
Adafruit_StepperMotor *myStepper5 = AFMStop.getStepper(200, 1); //stepper on backstop 3

//defining the two wrapper functions for the first backstop stepper motor here
void forwardstep3() {
  myStepper3->onestep(FORWARD, SINGLE); //using double here for more torque. Change to microstep for more precision
}
void backwardstep3() {
  myStepper3->onestep(BACKWARD, SINGLE);
}

//defining the two wrapper functions for the second backstop stepper motor here
void forwardstep4() {
  myStepper4->onestep(FORWARD, SINGLE);
}
void backwardstep4() {
  myStepper4->onestep(BACKWARD, SINGLE);
}

//defining the two wrapper functions for the third backstop stepper motor here
void forwardstep5() {
  myStepper5->onestep(FORWARD, SINGLE);
}

void backwardstep5() {
  myStepper5->onestep(BACKWARD, SINGLE);
}

//wrapping the two wrapper functions into an AccelStepper object
AccelStepper stepper3(forwardstep3, backwardstep3);
AccelStepper stepper4(forwardstep4, backwardstep4);
AccelStepper stepper5(forwardstep5, backwardstep5);

//defining variables for force sensitive resistor
int fsrPin = 0; //the FSR is connected to a0 on the Mega
int fsrReading = 0; //the analog reading for the FSR resistor divider

int input = 0; //checking test cases with this input

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while(!Serial); //waiting for serial port to start up
  
  //starting the three motor shields here
  AFMSbot.begin();
  AFMSmid.begin();
  AFMStop.begin();
  
  //starting the I2C here. Remove serial data code above when I2C 
  //starts working
  Wire.begin();

  //defining the max speed and acceleration for each motor here
  myStepper3->setSpeed(300);
  myStepper4->setSpeed(300.0);
  myStepper5->setSpeed(300);
}

void loop() {
  // put your main code here, to run repeatedly:
 switch(input) {
  case 1:
    Serial.println("Executing case 1");
    while(fsrReading < 100) {
      myStepper3->step(5, FORWARD, SINGLE);
      fsrReading = analogRead(fsrPin);
    }
    myStepper3->release();
    break;

  case 2:
    while(fsrReading < 100) {
      myStepper4->step(5, FORWARD, SINGLE);
      fsrReading = analogRead(fsrPin);
    }
    myStepper4->release();
    break;
  case 4:
    while(fsrReading < 100) {
      myStepper5->step(5, FORWARD, SINGLE);
      fsrReading = analogRead(fsrPin);
    }
    myStepper5->release();
    break;
 }
}


