#include "pitches.h"
#define LDR 0

int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G4, NOTE_F2, NOTE_G3, NOTE_A5, NOTE_B3, NOTE_C4
};
int duration = 500;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Serial monitor has started");
}

void loop() {
  // put your main code here, to run repeatedly:
  int light = analogRead(LDR);
  Serial.print("The light sensitivity is ");
  Serial.println(light);
  int val = light/24.142;
  tone(8, melody[val], duration);
  delay(2000);
  exit(0);
}
