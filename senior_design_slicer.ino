//Including all the required libraries here. Wire.h required for I2C intercommunication
#include <AccelStepper.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>

//Creating the Adafruit motor shield object. Not sure this is needed with the AccelStepper library
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

Adafruit_MotorShield AFMStop(0x60); //this is the default address for I2C communication

//Connecting the three steppers for the X(1&2) and Y(3) axes here. Motor is 200 steps/rev 
//* here is for declaring pointer for an instance of the class
Adafruit_StepperMotor *myStepper1 = AFMStop.getStepper(200, 1); //first stepper on the left
Adafruit_StepperMotor *myStepper2 = AFMStop.getStepper(200, 2); //second stepper on the right
Adafruit_StepperMotor *myStepper3 = AFMStop.getStepper(200, 3); //third stepper on X axis

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

//defining the two wrapper functions for the third stepper motor here. This is for y-axis
void forwardstep3() {
  myStepper2->onestep(FORWARD, DOUBLE);
}
void backwardstep3() {
  myStepper2->onestep(BACKWARD, DOUBLE);
}

//Wrapping the 3 steppers into an AccelStepper object
AccelStepper stepper1(forwardstep1, backwardstep1);
AccelStepper stepper2(forwardstep2, backwardstep2);
AccelStepper stepper3(forwardstep3, backwardstep3);

//defining variables here for motion along the X-axis.
double distance = 36; //the distance here is the total length of the X-slot. Distance in in
double distance_per_revolution = 0.2519; //the distance moved per revolution of the motor. MASSIVE ASSUMPTION!
double first_slot =  9.12; //the distance between axle of left motor and center of first slot
double second_slot = 16.12; //the distance between axle of left motor and center of second slot
double third_slot = 23.12; //the distance between axle of left motor and center of third slot
double Y_distance = 10.3; //distance the slicer will have to move forward to be flush with meat

//these variables define the number of revolutions that the motor will go through to reach
//each slot. These are defined as int because they all need to be whole numbers. The zero position
//is taken to be when the center of the slicer is right over the axle of the motor on the left
int first_slot_rev = first_slot/distance_per_revolution; //no of revolutions to reach 1st slot
int second_slot_rev = second_slot/distance_per_revolution; //no of revolutions to reach 2nd slot
int third_slot_rev = third_slot/distance_per_revolution; //no of revolutions to reach 3rd slot
int Y_distance_rev = Y_distance/distance_per_revolution; //no of revolutions to be flush with meat

//defining a variable here to accept a serial input and check if statement gets executed
int input = 0;

void setup() {
  //creating Serial here to accept input
  Serial.begin(9600);
  while(!Serial); //waiting for serial port to start up
  // put your setup code here, to run once:
  AFMStop.begin(); //start the top motor shield here

  //reading Serial data here
  Serial.println("Program has started");
  input = Serial.read();
  Serial.println(input);

  //starting the I2C here. Remove serial data code above when I2C 
  //starts working
  Wire.begin();

  //defining the max speed and acceleration for each motor here
  stepper1.setMaxSpeed(100.0);
  stepper1.setAcceleration(100.0);
  stepper2.setMaxSpeed(100.0);
  stepper2.setAcceleration(100.0);
  stepper3.setMaxSpeed(100.0);
  stepper3.setAcceleration(100.0);
}

//this function will take care of the back part of the back and forth motion
void back_motion() {
  int current = stepper1.currentPosition(); //get the current revolution count for motor1
  int back_distance = current - 15; //number of revolutions to move back by
  stepper1.moveTo(back_distance); 
  stepper2.moveTo(back_distance);
}

//this function will take care of the forth part of the back and forth motion
void forth_motion() {
  int current = stepper1.currentPosition(); //get the current revolution count for motor 1
  int forth_distance = current + 15;
  stepper1.moveTo(forth_distance);
  stepper2.moveTo(forth_distance);
}

void loop() {
  // put your main code here, to run repeatedly:
  stepper1.run();
  stepper2.run();
  stepper3.run();

  static char c = ''; //defining the char as a static var here

  //Request data from slave Arduino here
  Wire.requestFrom(8, 1); //requesting 1 byte of data to represent character
  while(Wire.available()) {
    c = Wire.read(); //Use this c in the switch case statement below
  }
  
  //switch case statements to move to the specific slot. Used 49, 50, 51 instead of 1, 2, 3
  //here because. Serial.read() spits out bit values, not the actual int values. 
  //Placing the switch statement here because currentPosition can only be queried from within 
  //the loop function
  switch(input) {
    case 49:
      stepper1.moveTo(first_slot_rev);
      stepper2.moveTo(first_slot_rev);
      Serial.println("Moving to the first slot");
      //this if statement gets executed when first slot gets hit
      if(stepper1.currentPosition() == first_slot_rev){
        forth_motion();
      }
      //once the forth motion has completed, back motion will initiate
      if(stepper1.currentPosition() == first_slot_rev + 15){
        back_motion();
      }
      break;
    case 50:
      stepper1.moveTo(second_slot_rev);
      stepper2.moveTo(second_slot_rev);
      Serial.println("Moving to the second slot");
      //if statement gets executed when second slot gets hit
      if(stepper1.currentPosition() == second_slot_rev){
        forth_motion();
      }
      //once the forth motion has completed, back motion will initiate
      if(stepper1.currentPosition() == second_slot_rev + 15){
        back_motion();
      }
      break;
    case 51:
      stepper1.moveTo(third_slot_rev);
      stepper2.moveTo(third_slot_rev);
      Serial.println("Moving to the third slot");
      //if statement gets executed when third slot gets hit
      if(stepper1.currentPosition() == third_slot_rev){
        forth_motion();
      }
      //once the forth motion has completed, back motion will initiate
      if(stepper1.currentPosition() == third_slot_rev + 15){
        back_motion();
      }
  }
}
