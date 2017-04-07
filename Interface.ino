////////// Include the necessary libraries //////////////////////////////////////

// Include the libraries with the built-in keypad functions
#include <Key.h>
#include <Keypad.h>
// Include the libraries with the built-in LCD functions
#include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>
// Include the library for I2C communication with other devices (LCD, Mega, etc.)
#include <Wire.h>

//defining available backlight colors
#define OFF 0x0
#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7 

////////// Initialize the keypad code ////////////////////////////////////////////

const byte ROWS = 4; // Create a "row" variable equal to 4,
                     // because there are 4 rows of keys on pad
const byte COLS = 3; // Create a "column" variable equal to 3,
                     // because there are 3 columns of keys on pad
char keys[ROWS][COLS] = {
  {'1', '2', '3'},  // Create a matrix whose elements correspond to key values on the real keypad.
  {'4', '5', '6'},  // This has to be a character ("char") array to be able to include * and #.
  {'7', '8', '9'},
  {'*', '0', '#'}
}; 
byte rowPins[ROWS] = {8, 7, 6, 5}; // This is where specific I/O pins on the Arduino
                                   // are assigned to represented certain rows and
byte colPins[COLS] = {4, 3, 2};    // columns of the matrix. A pair of these pins
                                   // activated simultaneously will represent a
                                   // specific key.

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS ); // Create a virtual object called "keypad" representing the keypad.

////////////// Initialize the LCD code //////////////////////////////////////

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield(); // Create a virtual object called "lcd" representing the LCD display.

////////////// Declare variables ////////////////////////////////////////////

char meat = '0'; // Initialize meat selection variable.
char slices[3] = {'n','0', '1'}; // Initialize array to hold weight input:
                             // first& second inputs/digits, respectively.
char reset = '0'; // Initialize a variable to serve as the reset command to LCD.
int slices_num = 0; 
char cancel = '0';
char meat_msg[3] = "m";

/////////////////////////////////////////////////////////////////////////////

void setup() {
  // The setup code runs one time when the Arduino is turned on.
  // It is essentially the boot-up code and should be used to
  // initialize hardware to the desired initial conditions.

  Serial.begin(9600); // Set speed and start serial communication. (Needed?)

  lcd.begin(16, 2); // Set up the LCD's number of columns and rows. This is a 16x2
                    // display, which means you are limited to 16-character messages
                    // on each line (including spaces).
  lcd.setBacklight(BLUE); // Turn on the backlight of the display,
                          // otherwise it's really hard to read the text.
  lcd.print(" -- AutoDeli -- "); // Prints message to the LCD. Note: Upon booting,
                                 // the display will default to the top left corner.
                                 // If you want to print somewhere else, you have to
                                 // specify where.
  lcd.setCursor(0, 1); // Set cursor all the way to left on the 2nd line of LCD.
  lcd.print("Press any key");

  Wire.begin(9); //setting up I2C serial bus here. This is master writing device #9

}

///////////////////////////////////////////////////////////////////////

void loop() {  
  // Wake up the machine.

beginning:  
  keypad.waitForKey(); // The program will remain frozen here until a key is pressed.

meat_input: 
  // Prompt user for meat selection.
  lcd.clear(); // Clear the LCD, otherwise you will be writing over the previous
               // message and have unwanted leftover characters.
  lcd.setBacklight(WHITE);
  lcd.setCursor(0, 0); // Set cursor to the top left corner of LCD.
  lcd.print("-Select a meat-");
  lcd.setCursor(0, 1); // Set cursor to the bottom left corner of LCD.
  lcd.print("(1 2 or 3): ");
  meat = keypad.waitForKey(); // The program will remain frozen on this screen
                              // until the user presses a key. The value will be
                              // returned to the "meat" variable.
  
  // Error handling for the meat selection.
  while (meat != '1' && meat != '2' && meat != '3') {
    lcd.clear();
    lcd.setBacklight(RED);
    lcd.setCursor(0, 0);
    lcd.print("Bad Selection");
    lcd.setCursor(0, 1);
    lcd.print("Select: (1)(2)(3) ");
    meat = keypad.waitForKey(); // Wait for a new user input.
  }

  meat_msg[1] = meat;
  Serial.println(meat_msg);
  
  //Writing to slave device #8, which is the Mega
  Wire.beginTransmission(8);
  Wire.write(meat_msg);
  Wire.endTransmission();
  
slices_input:
  // Prompt the user for desired quantity.
  lcd.setBacklight(WHITE); // Reset the screen to white (it will be red if there was a meat error).
  lcd.clear(); // Clear the LCD, otherwise you will be writing over the previous
               // message and have unwanted leftover characters.
  lcd.setCursor(0, 0); // Set cursor to the top left corner of LCD.
  lcd.print("Enter # of");
  lcd.setCursor(0, 1); // Set cursor to the bottom left corner of LCD.
  lcd.print("slices: "); // Print a field to 2nd line.
  lcd.setCursor(8, 1);
  slices[1] = keypad.waitForKey(); // Pause and wait for first digit.
  lcd.setCursor(9, 1);
  lcd.print(slices[1]); // Print the first digit entered.
  slices[2] = keypad.waitForKey(); // Pause and wait for second digit.
  
  if (slices[2] != '#'){ //This is for a double digit slice
    Serial.println("Entered this if statement");
    slices_num = (int(slices[1])-48) * 10 + (int(slices[2])) - 48;
    Serial.println(slices_num);
    lcd.setCursor(10, 1);
    lcd.print(slices[2]); // Print the second digit entered.
    delay(500); // Pause and wait 500 ms.
    
  }
  else if(slices[2] == '#') {
    slices[2] = "";
  } 
  else if(slices[2] == '*') {
    goto slices_input;
  }
  
  // Error handling for the slices input.
  if (slices_num > 25) //I am trying to set the limit for number of slices ordered at 25, this can be changed. 
    {
      lcd.setCursor(0,0);
      lcd.clear();
      lcd.print("25 slices");
      lcd.setCursor(0,1);
      lcd.print("or less         ");
      delay(2000); 
      goto slices_input;
    }

  //Sending the number of slices to the Mega
  Wire.beginTransmission(8);
  Wire.write(slices);
  Wire.endTransmission();

 
  // Confirmation screen for user to approve the inputs.
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Meat: ");
  lcd.setCursor(6,0);
  lcd.print(meat);
  lcd.setCursor(0,1);
  lcd.print("# Slices: ");
  lcd.setCursor(11,1);
  lcd.print(slices[1]);
  lcd.setCursor(12,1);
  lcd.print(slices[2]);

  //Wait for the user to approve or deny the order
  cancel =  keypad.waitForKey();

  //Waiting for user input of either # or * only
  while(cancel != '#' && cancel != '*')
  {
   cancel = keypad.waitForKey();
  }

  //The cancel option
  if (cancel == '*' )
  {
   goto meat_input;
  }

  //The accept option
  else if(cancel == '#') {
    Wire.beginTransmission(8);
    Wire.write("S");
    Wire.endTransmission();
  }

  Wire.requestFrom(8, 1);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Slicing...");
  while(Wire.read() != 'K') {
    Wire.requestFrom(8, 1);
  }
 
  /// When slicing is complete:
  lcd.clear(); // Clear the LCD, otherwise you will be writing over the previous
               // message and have unwanted leftover characters.
  lcd.setBacklight(GREEN);
  lcd.setCursor(0, 0); // Set cursor to the top left corner of LCD.
  lcd.print("Please retrieve");
  lcd.setCursor(0, 1); // Set cursor to the bottom left corner of LCD.
  lcd.print("your purchase."); // Print a field to 2nd line.

  // Press the # key to reset the LCD to the home screen.
  reset =  keypad.waitForKey(); // The program will remain frozen here until a key is pressed.

  //Running setup code again 
  lcd.setBacklight(BLUE); // Turn on the backlight of the display,
                          // otherwise it's really hard to read the text.
  lcd.setCursor(0, 0);                        
  lcd.print(" -- AutoDeli -- "); // Prints message to the LCD. Note: Upon booting,
                                 // the display will default to the top left corner.
                                 // If you want to print somewhere else, you have to
                                 // specify where.
  lcd.setCursor(0, 1); // Set cursor all the way to left on the 2nd line of LCD.
  lcd.print("Press any key");

}

