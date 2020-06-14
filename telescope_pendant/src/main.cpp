#include <Arduino.h>
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include <string.h>
/*
Not all pins on the micro support change interrupts,
 so only the following can be used for RX: 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).
*/
//Pin Defs
  #define baud 9600
  #define lcdRs 13
  #define lcdEn 12
  #define db7 11
  #define db6 10
  #define db5 9
  #define db4 8 
  LiquidCrystal lcd(lcdRs, lcdEn, db4, db5, db6, db7); //inits the lcd
  //softserial for mega
  #define fromMega 14
  #define toMega   16
  SoftwareSerial mega (fromMega, toMega); 
  //function switch
  #define fxSw   6 //pin 1/3 of the sp3t switch 
  #define fxSw1  5 //pin 3/3 of the sp3t switch
                  //note: the sp3t switch has its middle pin disconnected
  //resolution switch
  #define resSw  7 //res switch on pin 7
  //joystick
  #define joy0  18
  #define joy1  19
 

void setup() {
  // set pins as input with internal pull-up resistors enabled

  //other inits
  mega.begin(baud); //starts b-d serial with mega
  mega.write("0000");
  lcd.begin(20, 4);
  lcd.print("INIT");
  lcd.clear();
}
//function time
float scl(float data) { //for scaling the joystick inputs
  float scale = 10;
  float scl_data = 0;
  scl_data = data / scale;
  return scl_data;
}
bool move(float data){ // used to create a deadzone if you're into that
  float deadzone = 1;
  if(data >= deadzone){
    return true;
  } else {
    return false; 
  }
}
void updateLcd(String title, String val){
  int len = title.length();
  lcd.setCursor(0,0);
  lcd.print(title);
  lcd.setCursor((len + 1), 0);
}
void loop() {
int fxSw_S = 0;
int resSw_S = 0;
float joy0_v = 0;
float joy1_v = 0;
int sentMode = 0;
//char mode[12] = "";
  //Handling the function switch
  if(digitalRead(fxSw) == HIGH){
    fxSw_S = 0;
    //mode = "Declination"; 
  } else if (digitalRead(fxSw1) == HIGH){
    fxSw_S = 1;
    //mode = "Ascention";
  } else if (digitalRead(fxSw) == LOW && digitalRead(fxSw1) == LOW){
    fxSw_S = 2;
    //mode = "Focus";
  }
  //Handling the resolution switch
  if(digitalRead(resSw) == HIGH){
    resSw_S = 0;
  }else if (digitalRead(resSw) == HIGH) {
    resSw_S = 1;
  }
  //LCD Output section
  if (fxSw_S == 0 && resSw_S == 0){
    //dec coarse
    updateLcd("Dec Coarse", "pl");
    sentMode = 1; 
  } else if (fxSw_S == 1 && resSw_S == 0) {
    //asc coarse
    updateLcd("Asc Coarse", "pl");
    sentMode = 2;
  } else if (fxSw_S == 2 && resSw_S == 0) {
    //foc coarse
    updateLcd("Focus Coarse", " "); //focus is qualitiative for all purposes of this program
    sentMode = 3;
  } else if (fxSw_S == 0 && resSw_S == 1){
    //dec fine
    updateLcd("Dec Fine","pl");
    sentMode = 4;
  } else if (fxSw_S == 1 && resSw_S == 1){
    //asc fine
    updateLcd("Asc Fine", "pl");
    sentMode = 5;
  } else if (fxSw_S == 2 && resSw_S == 1){
    //foc fine
    updateLcd("Focus Fine", " "); // ibid
    sentMode = 6;
  }
  //joystick
    joy0_v = analogRead(joy0);
    delay(100); 
    joy1_v = analogRead(joy1);
  //comminucation to mega
  if (sentMode == 0){ //verifies that valid data will be sent
    if (sentMode >= 7){
        mega.write(sentMode);
    }
  }
}

