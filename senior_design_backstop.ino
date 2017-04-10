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
Adafruit_StepperMotor *myStepper5 = AFMStop.getStepper(200, 3); //stepper on backstop 3

//defining the two wrapper functions for the first backstop stepper motor here
void forwardstep3() {
  myStepper3->onestep(FORWARD, DOUBLE); //using double here for more torque. Change to microstep for more precision
}
void backwardstep3() {
  myStepper3->onestep(BACKWARD, DOUBLE);
}

//defining the two wrapper functions for the second backstop stepper motor here
void forwardstep4() {
  myStepper4->onestep(FORWARD, DOUBLE);
}
void backwardstep4() {
  myStepper4->onestep(BACKWARD, DOUBLE);
}

//defining the two wrapper functions for the third backstop stepper motor here
void forwardstep5() {
  myStepper5->onestep(FORWARD, DOUBLE);
}

void backwardstep5() {
  myStepper5->onestep(BACKWARD, DOUBLE);
}

//wrapping the two wrapper functions into an AccelStepper object
AccelStepper stepper3(forwardstep3, backwardstep3);
AccelStepper stepper4(forwardstep4, backwardstep4);
AccelStepper stepper5(forwardstep5, backwardstep5);

//defining variables for motion of backstop
double distance_per_step = 0.001574803; //the distance moved per step of the motor in inches. MASSIVE ASSUMPTION!
double slot_length = 9; //the maximum distance the backstop can move in inches (still need to measure for exact value)
int lead_screw_steps = 400; //initial no. of steps for backstops
int max_steps = slot_length/distance_per_step; //no. of steps for backstop to travel full length of lead screw
int total_steps = 0; //total no. of steps the backstop has traveled 

//defining variables for force sensitive resistor
int fsrPin = 0; //the FSR is connected to a0 on the Mega
int fsrReading; //the analog reading for the FSR resistor divider

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
  stepper3.setMaxSpeed(100.0);
  stepper3.setAcceleration(100.0);
  stepper4.setMaxSpeed(100.0);
  stepper4.setAcceleration(100.0);
  stepper5.setMaxSpeed(100.0);
  stepper5.setAcceleration(100.0);
}

void loop() {
  // put your main code here, to run repeatedly:
  stepper3.run();
  stepper4.run();
  stepper5.run();
  
  //switch(input) {
      
    //case 1:
      //stepper1.moveTo(first_slot_rev);
      //Serial.println("Moving to the first slot");
      //this if statement gets executed when first slot gets hit
      //if(stepper1.currentPosition() == first_slot_rev){
        //Serial.println("First target hit");
        //stepper2.moveTo(Y_distance_rev);
        //if(stepper2.currentPosition() == Y_distance_rev){
          fsrReading = analogRead(fsrPin);  
          Serial.print("Analog reading = ");
          Serial.print(fsrReading);     // the raw analog reading
          total_steps = 0; //
          while (fsrReading < 200 && total_steps < max_rev) {
            if (fsrReading == 0) {
              lead_screw_steps = 400;
            }
            else {
              lead_screw_steps = 10;
            }
            stepper3.moveTo(lead_screw_rev); //moves slot 1 backstop if FSR is below 200
            total_steps = total_steps + lead_screw_steps;
            fsrReading = analogRead(fsrPin); //updates the reading 
          }
            Serial.println("meat in contact with slicer");
          //return_value = back_and_forth_motion(stepper1.currentPosition()); 
       // }
      stepper3.moveTo(0); //returns slot 1 backstop to home position
      //}
      //break;
    //case 2:
      //stepper1.moveTo(second_slot_rev);
      //Serial.println("Moving to the second slot");
      //if statement gets executed when second slot gets hit
      //if(stepper1.currentPosition() == second_slot_rev){
          fsrReading = analogRead(fsrPin);  
          Serial.print("Analog reading = ");
          Serial.print(fsrReading);     // the raw analog reading
          total_steps = 0;
          while (fsrReading < 200 && total_steps < max_rev) {
            if (fsrReading == 0) {
              lead_screw_steps = 400;
            }
            else {
              lead_screw_steps = 10;
            }
            stepper4.moveTo(lead_screw_rev); //moves slot 2 backstop if FSR is below 200
            total_steps = total_steps + lead_screw_steps;
            fsrReading = analogRead(fsrPin); //updates the reading 
          }
            Serial.println("meat in contact with slicer");
          //return_value = back_and_forth_motion(stepper1.currentPosition()); 
       // }
      stepper4.moveTo(0); //returns slot 2 backstop to home position
        //}
      //break;
  //case 3:
      //stepper1.moveTo(third_slot_rev);
      //Serial.println("Moving to the third slot");
      //if statement gets executed when third slot gets hit
      //if(stepper1.currentPosition() == third_slot_rev){
          fsrReading = analogRead(fsrPin);  
          Serial.print("Analog reading = ");
          Serial.print(fsrReading);     // the raw analog reading
          total_steps = 0;
          while (fsrReading < 200 && total_steps < max_rev) {
            if (fsrReading == 0) {
              lead_screw_steps = 400;
            }
            else {
              lead_screw_steps = 10;
            }
            stepper5.moveTo(lead_screw_rev); //moves slot 2 backstop if FSR is below 200
            total_steps = total_steps + lead_screw_steps;
            fsrReading = analogRead(fsrPin); //updates the reading 
          }
            Serial.println("meat in contact with slicer");
          //return_value = back_and_forth_motion(stepper1.currentPosition()); 
       // }
      stepper5.moveTo(0); //returns slot 2 backstop to home position
      //}
//}

}
