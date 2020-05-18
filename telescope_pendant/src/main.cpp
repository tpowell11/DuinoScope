#include <Arduino.h>
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include <string.h>
//Pin Defs
  #define baud 9600
  #define lcdRs 1
  #define lcdEn 2
  #define lcd0  3
  #define lcd1  4
  #define lcd2  5
  #define lcd3  6
  #define lcd4  7
  LiquidCrystal lcd(lcdRs, lcdEn, lcd0, lcd1, lcd2, lcd3);
  #define fromMega 14
  #define toMega   16
  SoftwareSerial mega (fromMega, toMega); 
  //function switch
  #define fxSw   10
  #define fxSw1  11
  //resolution switch
  #define resSw  8
  #define resSw1 9
  //encoder coding lines
  #define dial0  12
  #define dial1  13

void setup() {
  mega.begin(baud);
  lcd.begin(20, 4);
  lcd.print("INIT");
}


void loop() {
int fxSw_S = 0;
int resSw_S = 0;
//char mode[12] = "";
  //Handling the function switch
  if(digitalRead(fxSw) == HIGH){
    fxSw_S = 0;
    //mode = "Declination"; 
  } else if (digitalRead(fxSw1) == HIGH){
    fxSw_S = 1;
    //mode = "Ascention";
  } else if (digitalRead(fxSw) == HIGH && digitalRead(fxSw1) == HIGH){
    fxSw_S = 2;
    //mode = "Focus";
  }
  //Handling the resolution switch
  if(digitalRead(resSw) == HIGH){
    resSw_S = 0;
  }else if (digitalRead(resSw) == HIGH) {
    resSw_S = 1;
  }else if (digitalRead(resSw) == HIGH && digitalRead(resSw1) == HIGH){
    resSw_S = 2; 
  }
}