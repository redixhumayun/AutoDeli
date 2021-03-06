//Including all the required libraries here. Wire.h required for I2C intercommunication
#include <AccelStepper.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

//Creating the Adafruit motor shield object. Not sure this is needed with the AccelStepper library
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

Adafruit_MotorShield AFMSbot(0x63); //this is the address for the top shield
Adafruit_MotorShield AFMSmid(0x61); //this is the for the middle shield
Adafruit_MotorShield AFMStop(0x62); //this is for the top shiled


//Connecting the two steppers for X&Y here. Motor is 200 steps/rev 
//* here is for declaring pointer for an instance of the class
Adafruit_StepperMotor *myStepper1 = AFMSbot.getStepper(200, 1); //stepper on the X
Adafruit_StepperMotor *myStepper2 = AFMSbot.getStepper(200, 2); //stepper on the Y
Adafruit_StepperMotor *myStepper3 = AFMSmid.getStepper(200, 1); //stepper on backstop 1
Adafruit_StepperMotor *myStepper4 = AFMSmid.getStepper(200, 2); //stepper on backstop 2
Adafruit_StepperMotor *myStepper5 = AFMStop.getStepper(200, 1); //stepper on backstop 3


//defining the two wrapper functions for the first stepper motor here
void forwardstep1() {
  myStepper1->onestep(FORWARD, SINGLE); //using double here for more torque. Change to microstep for more precision
}
void backwardstep1() {
  myStepper1->onestep(BACKWARD, SINGLE);
}

//defining the two wrapper functions for the second stepper motor here
void forwardstep2() {
  myStepper2->onestep(BACKWARD, SINGLE);
}
void backwardstep2() {
  myStepper2->onestep(FORWARD, SINGLE);
}

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


//Wrapping the 3 steppers into an AccelStepper object
AccelStepper stepper1(forwardstep1, backwardstep1);
AccelStepper stepper2(forwardstep2, backwardstep2);
AccelStepper stepper3(forwardstep3, backwardstep3);
AccelStepper stepper4(forwardstep4, backwardstep4);
AccelStepper stepper5(forwardstep5, backwardstep5);

//defining fsrResistor here
int fsrPin = 0; //the FSR is connected to a0 on the Mega
int fsrReading; //the analog reading for the FSR resistor divider

//defining variables here for motion along the X-axis.
double distance_per_revolution = 0.008; //the distance moved per revolution of the motor.
double first_slot =  0; //the distance between axle of left motor and center of first slot
double second_slot = 7; //the distance between axle of left motor and center of second slot
double third_slot = 14; //the distance between axle of left motor and center of third slot

//variables for motion along the Y-axis
double Y_distance = 1.5; //distance the slicer will have to move forward to cut meat 
double Y_distance_push_back = 0.75; //distance to push meat back into the meat slot

//no of revolutions to reach 1st slot
int first_slot_rev = first_slot/distance_per_revolution; 
//no of revolutions to reach 2nd slot
int second_slot_rev = second_slot/distance_per_revolution; 
//no of revolutions to reach 3rd slot
int third_slot_rev = third_slot/distance_per_revolution; 
//no of revolutions to be flush with meat
int Y_distance_rev = Y_distance/distance_per_revolution; 
//no of revolutions to push meat back into meat slot
int Y_distance_push_back_rev = Y_distance_push_back/distance_per_revolution;


//variables for simulating back and forth motion go here
double simulation_width = 9.75; //random value here. Replace with actual value
//no of revolutions to simulate back and forth motion on either side
int simulation_distance = simulation_width/distance_per_revolution;

//defining variables here to accept input from the Arduino Uno
char c = '0';
int meat = 0;
char slices[2] = {};
int num_of_slices = 0; //will contain integer value of slices char array
int start_signal = 0; //variable to hold the start signal

int return_value = 0; //variable to check when back_and_forth func has returned

boolean done_flag = false; //flag to keep track of whether done slicing or not. 0 is false

//variable to keep track of backstop movement
int total_steps = 0;

//defining end stop variables here
int x_pin = 22; //pin number for end stop on X axis
int end_stop_pin = 23; //pin number for end stop on 3rd slot

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
  stepper1.setMaxSpeed(350.0);
  stepper1.setAcceleration(600.0);
  stepper2.setMaxSpeed(200.0);
  stepper2.setAcceleration(100.0);
  myStepper3->setSpeed(300); //rpm
  myStepper4->setSpeed(300); //rpm
  myStepper5->setSpeed(300); //rpm
}

void getValues() {
  while(Wire.available()){
    c = Wire.read();
    Serial.println(c);
    switch(c) {
      case 'm': //case to get the meat
        Serial.println("case 1 got executed");
        meat = Wire.read() - 48; //subtracting 48 to get the actual value
        Serial.println(meat);
        Serial.println(Wire.available());
        Serial.println("------");
        break;

      case 'n': //case to get the number of slices
        Serial.println("case 2 got executed");
        //Serial.println(Wire.available());
        slices[0] = Wire.read();
        slices[1] = Wire.read();
        num_of_slices = atoi(slices);
        Serial.println(num_of_slices);
        Serial.println("------");
        break;

      case 'S': //case to start the actual slicing
        Serial.println("case 3 got executed");
        start_signal = 1;
        Serial.print("Here is the start signal: ");
        Serial.println(start_signal);
        Serial.println(Wire.available());
        break;
    }
  }
}

  
int back_and_forth_motion() { //function that will simulation slicing
  if(num_of_slices > 0) { //checks if it needs to slice
    int current_pos = stepper1.currentPosition();
    stepper1.runToNewPosition(current_pos + simulation_distance);
    stepper1.runToNewPosition(current_pos);
    num_of_slices = num_of_slices - 1;
    back_and_forth_motion();
  }else { //if done slicing return from function and move Y back to 0
    return 1;
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  stepper1.run();
  stepper2.run();
  
  //switch case statements with 1,2,3 representing each meat slot 
  //Placing the switch statement here because currentPosition can only be queried from within 
  //the loop function
  if(start_signal == 1) {
    switch(meat) { //need to move repetitive code within another function   
      case 1:
      //first two lines move the X and Y steppers
        stepper1.moveTo(first_slot_rev);
        stepper2.moveTo(Y_distance_rev);
        if(stepper1.currentPosition() == first_slot_rev && stepper2.currentPosition() == Y_distance_rev){
         while(fsrReading < 100) { //while loop to move backstops until fsr hit
          myStepper3->step(5, FORWARD, SINGLE); //moves using MotorShield V2 lib
          total_steps = total_steps + 5;
          fsrReading = analogRead(fsrPin); //takes FSR reading
         }
         stepper2.runToNewPosition(stepper2.currentPosition() - 20);
         return_value = back_and_forth_motion(); //calls slicing simulation function
         myStepper3->step(total_steps, BACKWARD, SINGLE);
        }
        break;
      case 2:
        stepper1.moveTo(second_slot_rev);
        stepper2.moveTo(Y_distance_rev);
        if(stepper1.currentPosition() == second_slot_rev && stepper2.currentPosition() == Y_distance_rev){
          while(fsrReading < 100) {
            myStepper4->step(5, FORWARD, SINGLE);
            total_steps = total_steps + 5;
            fsrReading = analogRead(fsrPin);
          }
          stepper2.runToNewPosition(stepper2.currentPosition() - 20);
          return_value = back_and_forth_motion();
          myStepper4->step(total_steps, BACKWARD, SINGLE);
        }
        break;
      case 3:
        stepper1.moveTo(third_slot_rev);
        stepper2.moveTo(Y_distance_rev);
        if(stepper1.currentPosition() == third_slot_rev && stepper2.currentPosition() == Y_distance_rev){
          while(fsrReading < 100) {
            if(digitalRead(end_stop_pin) == 0) {
              return_value = 1;
              break;
            }
            myStepper5->step(5, FORWARD, SINGLE);
            total_steps = total_steps + 5;
            fsrReading = analogRead(fsrPin);
          }
          stepper2.runToNewPosition(stepper2.currentPosition() - 20);
          if(digitalRead(end_stop_pin) != 0) {
            return_value = back_and_forth_motion();
          }
          myStepper5->step(total_steps, BACKWARD, SINGLE);
        }
        break;
    }
  }
    
  

  if(return_value == 1) { //checks to see if slicing is done or not
    meat = 0; //changing the input here to prevent loop function from running indef

    stepper2.runToNewPosition(stepper2.currentPosition() + Y_distance_push_back_rev + 20); //pushes meat back into slot
    stepper2.runToNewPosition(0); //moves Y back to 0
    
    while(digitalRead(x_pin) == 1) {
      //myStepper1->setSpeed(100);
      //myStepper1->step(5, BACKWARD, SINGLE);
      stepper1.moveTo(-300);
      stepper1.setSpeed(-300);
      stepper1.runSpeedToPosition();
    }
    stepper1.setCurrentPosition(0);

    done_flag = true; //setting done flag to true to send back signal 
    Serial.print("Changed the value of done_flag");
    Serial.println(done_flag);
    releaseAllMotors();
    

  }
}

void releaseAllMotors() {
  myStepper1->release();
  myStepper2->release();
  myStepper3->release();
  myStepper4->release();
  myStepper5->release();
  

  //resetting all the variables here to 0 
  meat = 0;
  num_of_slices = 0;
  start_signal = 0;
  return_value = 0;
  fsrReading = 0;
  total_steps = 0;
}

void requestEvent(){
  Serial.println(done_flag);
  if (done_flag == true) {
    Wire.write("K");
    Serial.println("K was sent");
    done_flag = false;
  }
}
