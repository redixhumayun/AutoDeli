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
Adafruit_StepperMotor *myStepper1 = AFMSmid.getStepper(200, 1); //stepper on backstop 1
Adafruit_StepperMotor *myStepper2 = AFMSmid.getStepper(200, 2); //stepper on backstop 2
Adafruit_StepperMotor *myStepper3 = AFMStop.getStepper(200, 3); //stepper on backstop 3

//defining the two wrapper functions for the first stepper motor here
void forwardstep1() {
  myStepper1->onestep(FORWARD, DOUBLE); //using double here for more torque. Change to microstep for more precision
}
void backwardstep1() {
  myStepper1->onestep(BACKWARD, DOUBLE);
}

//defining the two wrapper functions for the second stepper motor here
void forwardstep2() {
  myStepper2->onestep(FORWARD, DOUBLE);
}
void backwardstep2() {
  myStepper2->onestep(BACKWARD, DOUBLE);
}

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
