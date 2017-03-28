
int x_home = 0; // Initialize a variable to hold the status of the switch.
                // The value is not important because it will be overwritten
                // in the loop before the value matters.

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(2, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

  x_home = digitalRead(2); // Check the status of the switch. 
                           // If the switch is pressed, it will return 0. 
                           // Otherwise, it will return a 1.
  if (x_home == 0){ // Check if the switch is pressed. If it is:
    Serial.println("Pressed"); // Print a message back to the computer.
    x_home = 1; // Reset the status variable of the switch.
  }
}
