
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


//Connecting the two steppers for X&Y here. Motor is 200 steps/rev 
//* here is for declaring pointer for an instance of the class
Adafruit_StepperMotor *myStepper1 = AFMSbot.getStepper(200, 1); //stepper on the X
Adafruit_StepperMotor *myStepper2 = AFMSbot.getStepper(200, 2); //stepper on the Y

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


//Wrapping the 3 steppers into an AccelStepper object
AccelStepper stepper1(forwardstep1, backwardstep1);
AccelStepper stepper2(forwardstep2, backwardstep2);

//defining variables here for motion along the X-axis.
double distance = 36; //the distance here is the total length of the X-slot. Distance in in
double distance_per_revolution = 0.0099173; //the distance moved per revolution of the motor. MASSIVE ASSUMPTION!
double first_slot =  9.12; //the distance between axle of left motor and center of first slot
double second_slot = 16.12; //the distance between axle of left motor and center of second slot
double third_slot = 23.12; //the distance between axle of left motor and center of third slot

//variables for motion along the Y-axis
double Y_distance = 10.3; //distance the slicer will have to move forward to be flush with meat


//these variables define the number of revolutions that the motor 
//will go through to reach each slot. 
//These are defined as int because they all need to be whole numbers. 
//The zero position is taken to be when the center of the slicer that is right over 
//the axle of the motor on the left

//no of revolutions to reach 1st slot
int first_slot_rev = first_slot/distance_per_revolution; 
//no of revolutions to reach 2nd slot
int second_slot_rev = second_slot/distance_per_revolution; 
//no of revolutions to reach 3rd slot
int third_slot_rev = third_slot/distance_per_revolution; 
//no of revolutions to be flush with meat
int Y_distance_rev = Y_distance/distance_per_revolution; 


//variables for simulating back and forth motion go here
double slot_width = 6.5; //random value here. Replace with actual value
double half_slot_width = slot_width/2; //to simulate back and forth motion
//no of revolutions to simulate back and forth motion on either side
int simulation_distance = half_slot_width/distance_per_revolution;

//defining variables here to accept input from the Arduino Uno
char c = '0';
int meat = 0;
char slices[2] = {};
int num_of_slices = 0; //will contain integer value of slices char array
char start_signal = "";

int return_value = 0; //variable to check when back_and_forth func has returned

void setup() {
  //creating Serial here to accept input
  Serial.begin(9600);
  while(!Serial); //waiting for serial port to start up

  //starting the three motor shields here
  AFMSbot.begin();
  AFMSmid.begin();
  AFMStop.begin();
  
  //starting the I2C here as slave device #8.
  Wire.begin(8);
  Wire.onReceive(getValues);
  Wire.onRequest(requestEvent);

  //defining the max speed and acceleration for each motor here
  stepper1.setMaxSpeed(100.0);
  stepper1.setAcceleration(100.0);
  stepper2.setMaxSpeed(100.0);
  stepper2.setAcceleration(100.0);
}

void getValues() {
  while(Wire.available()){
    c = Wire.read();
    Serial.println(c);
    switch(c) {
      case 'm': 
        Serial.println("case 1 got executed");
        meat = Wire.read() - 48; //subtracting 48 to get the actual value
        Serial.println(meat);
        Serial.println(Wire.available());
        Serial.println("------");
        break;

      case 'n':
        Serial.println("case 2 got executed");
        //Serial.println(Wire.available());
        slices[0] = Wire.read();
        slices[1] = Wire.read();
        num_of_slices = atoi(slices);
        Serial.println("------");
        break;

      case 'S':
        Serial.println("case 3 got executed");
        start_signal = c;
        break;
    }
  }
}

  
int back_and_forth_motion() {
  Serial.println("The number of slices is ");
  Serial.println(num_of_slices);
  int current_pos = stepper1.currentPosition();
  
  stepper1.runToNewPosition(current_pos + simulation_distance);
  stepper1.runToNewPosition(current_pos - simulation_distance);
  if(num_of_slices > 0) {
    num_of_slices = num_of_slices - 1;
    back_and_forth_motion();
  }
  return 1;
}

void loop() {
  // put your main code here, to run repeatedly:
  stepper1.run();
  stepper2.run();
  
  //switch case statements with 1,2,3 representing each meat slot 
  //Placing the switch statement here because currentPosition can only be queried from within 
  //the loop function
  switch(meat) {
      
    case 1:
      stepper1.moveTo(first_slot_rev);
      //this if statement gets executed when first slot gets hit
      Serial.println("case 1");
      if(stepper1.currentPosition() == first_slot_rev){
       return_value = back_and_forth_motion();
      }
      break;
    case 2:
      stepper1.moveTo(second_slot_rev);
      Serial.println("case 2");
      //if statement gets executed when second slot gets hit
      if(stepper1.currentPosition() == second_slot_rev){
        return_value = back_and_forth_motion();
      }
    break;
  case 3:
      stepper1.moveTo(third_slot_rev);
      Serial.println("case 3");
      //if statement gets executed when third slot gets hit
      if(stepper1.currentPosition() == third_slot_rev){
        return_value = back_and_forth_motion();
      }
    }


  if(return_value == 1) {
    stepper1.runToNewPosition(0);
    stepper2.runToNewPosition(0);
    meat = 0; //changing the input here to prevent loop function from running indef
    Serial.println("Returned Value");
  }
}

void requestEvent(){
    Wire.write("K");
  }
 
