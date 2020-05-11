#include <Arduino.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <Wire.h>
//pin defs
  #define lcd1 2
  #define lcd2 3
  #define lcd3 4
  #define lcd4 5
  #define lcd5 6
  #define lcd6 7
  #define enc0 8
  #define enc1 9
  #define fxnSw0 10
  #define fxnSw1 11
  #define resSw0 12
  #define resSw1 13
//serial to mega
  #define txPin 0
  #define rxPin 1
  SoftwareSerial serialToMega = SoftwareSerial(rxPin, txPin); 
//vars
  
LiquidCrystal lcd(lcd1, lcd2, lcd3, lcd4, lcd5, lcd6);
void setup() {
  // put your setup code here, to run once:
}

void loop() {
  int currFx = 0;
  int currRes = 0;
//interface runtime stuff
  if(digitalRead(fxnSw0) == HIGH){
    currFx = 1;
  }
  if(digitalRead(fxnSw1) == HIGH){
    currFx = 2;
  }
}