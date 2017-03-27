#include <Wire.h>

void setup() {
  // put your setup code here, to run once:
  Wire.begin(8);
  Wire.onRequest(requestEvent);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(100);
}

void requestEvent(){
  Wire.write("1");
}

