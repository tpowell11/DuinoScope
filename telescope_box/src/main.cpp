#include<Arduino.h>
#include<SoftwareSerial.h>
#include<Wire.h>
#include <SPI.h>/* Include the SPI library for the arduino boards */
#include <Stepper.h>
//encoder pin defs
  

  /* SPI commands */
  #define AMT22_NOP       0x00
  #define AMT22_RESET     0x60
  #define AMT22_ZERO      0x70

  /* Define special ascii characters */
  #define NEWLINE         0x0A
  #define TAB             0x09

  /* We will use these define macros so we can write code once compatible with 12 or 14 bit encoders */
  #define RES12           12
  #define RES14           14

  /* SPI pins */
  #define ENC_0            2 //was 2
  #define ENC_1            3 //was 3
  #define SPI_MOSI        11 //was 11
  #define SPI_MISO        12 //was 12
  #define SPI_SCLK        13 //was 13 
//ui pin defs
  #define estop       44
  #define heatled     46
  #define trackled    48
  #define warnpiezo   47
  //#define extcommled  49 temp. depreciated
  //#define contconnect 45 temp. depreciated
  #define estopLed    45
//motion control pin defs
  #define ascHbridge1  2
  #define ascHbridge2  3
  #define decHbridge1  4
  #define decHbridge2  5
  #define focusHbrige1 6
  #define focusHbrige2 7
  #define asclim1      17
  #define asclim2      18
  #define declim1      19
//micro communication defs
  #define toMicro   18
  #define fromMicro 19
  #define pendantBaud 9600
  SoftwareSerial serialPendant(fromMicro, toMicro);
  
//gps pins
  #define gpsRx 20
  #define gpsTx 21
  #define onePps 22
  #define gpsBaud 4800
  SoftwareSerial serialGPS(gpsRx, gpsTx);
// focus stepper cfg IMPORTANT: designed for BIPOLAR stepper. 
  #define SPR 200 //change to match the stepsper revolution of your stepper
  #define sPos1 
  Stepper myStepper(SPR, 8, 9, 10, 11);
void setup() {
  Serial.begin(9600); //begins main serial to computer / raspi
  //encoder setup
  Serial.print("DS Version b-1.0\nStarting\n"); 
    //Set the modes for the SPI IO
    pinMode(SPI_SCLK, OUTPUT);
    pinMode(SPI_MOSI, OUTPUT);
    pinMode(SPI_MISO, INPUT);
    pinMode(ENC_0, OUTPUT);
    pinMode(ENC_1, OUTPUT);
        
   //Get the CS line high which is the default inactive state
    digitalWrite(ENC_0, HIGH);
    digitalWrite(ENC_1, HIGH);

    //set the clockrate. Uno clock rate is 16Mhz, divider of 32 gives 500 kHz.
    //500 kHz is a good speed for our test environment
    SPI.setClockDivider(SPI_CLOCK_DIV32);    // 500 kHz
    //start SPI bus
    SPI.begin();
  //ui pins
    pinMode(heatled, OUTPUT);
    pinMode(trackled, OUTPUT);
    pinMode(warnpiezo, OUTPUT);
    //pinMode(extcommled, OUTPUT); temp. depreciated
    //pinMode(contconnect, OUTPUT); temp. depreciated
    pinMode(estopLed, OUTPUT);
  //motion control pins
    pinMode(ascHbridge1, OUTPUT);
    pinMode(ascHbridge2, OUTPUT);
    pinMode(decHbridge1, OUTPUT);
    pinMode(decHbridge2, OUTPUT);
    pinMode(focusHbrige1, OUTPUT);
    pinMode(focusHbrige2, OUTPUT);
    pinMode(asclim1, INPUT);
    pinMode(asclim2, INPUT);
    pinMode(declim1, INPUT);
  //gps pins & setup
    pinMode(gpsRx, INPUT);
    pinMode(gpsTx, OUTPUT);
    serialGPS.begin(gpsBaud);
    Serial.print("Init GPS\n"); //sends message to main console
    digitalWrite(gpsTx,HIGH);
      // Cut first gibberish
    while(serialGPS.available())
      if (serialGPS.read() == '\r')
        break;
  //micro communication pins, cfg, init
  serialPendant.begin(pendantBaud);
  Serial.print("Init Pendant\n"); //sends message to main console
  int pendIncTest = 0; //stores data incoming from pendant test procedure
  pendIncTest = serialPendant.read();
  if (pendIncTest == 1){
    // exit and continue setup
    Serial.print("Serial to pendant good\n");
  } else {
    // print error to pi via USB terminal
    Serial.print("ERR 1: Pendant not responding via serial\n");
  }

}
//function time
  void setCSLine (uint8_t encoder, uint8_t csLine){
      digitalWrite(encoder, csLine);
    }
  uint8_t spiWriteRead(uint8_t sendByte, uint8_t encoder, uint8_t releaseLine){
      //holder for the received over SPI
      uint8_t data;

      //set cs low, cs may already be low but there's no issue calling it again except for extra time
      setCSLine(encoder ,LOW);

      //There is a minimum time requirement after CS goes low before data can be clocked out of the encoder.
      //We will implement that time delay here, however the arduino is not the fastest device so the delay
      //is likely inherantly there already
      delayMicroseconds(3);

      //send the command  
      data = SPI.transfer(sendByte);
      delayMicroseconds(3); //There is also a minimum time after clocking that CS should remain asserted before we release it
      setCSLine(encoder, releaseLine); //if releaseLine is high set it high else it stays low
      
      return data;
    }
/*
    * This function gets the absolute position from the AMT22 encoder using the SPI bus. The AMT22 position includes 2 checkbits to use
    * for position verification. Both 12-bit and 14-bit encoders transfer position via two bytes, giving 16-bits regardless of resolution.
    * For 12-bit encoders the position is left-shifted two bits, leaving the right two bits as zeros. This gives the impression that the encoder
    * is actually sending 14-bits, when it is actually sending 12-bit values, where every number is multiplied by 4. 
    * This function takes the pin number of the desired device as an input
    * This funciton expects res12 or res14 to properly format position responses.
    * Error values are returned as 0xFFFF
    */
    uint16_t getPositionSPI(uint8_t encoder, uint8_t resolution){
      uint16_t currentPosition;       //16-bit response from encoder
      bool binaryArray[16];           //after receiving the position we will populate this array and use it for calculating the checksum

      //get first byte which is the high byte, shift it 8 bits. don't release line for the first byte
      currentPosition = spiWriteRead(AMT22_NOP, encoder, false) << 8;   

      //this is the time required between bytes as specified in the datasheet.
      //We will implement that time delay here, however the arduino is not the fastest device so the delay
      //is likely inherantly there already
      delayMicroseconds(3);

      //OR the low byte with the currentPosition variable. release line after second byte
      currentPosition |= spiWriteRead(AMT22_NOP, encoder, true);        

      //run through the 16 bits of position and put each bit into a slot in the array so we can do the checksum calculation
      for(int i = 0; i < 16; i++) binaryArray[i] = (0x01) & (currentPosition >> (i));

      //using the equation on the datasheet we can calculate the checksums and then make sure they match what the encoder sent
      if ((binaryArray[15] == !(binaryArray[13] ^ binaryArray[11] ^ binaryArray[9] ^ binaryArray[7] ^ binaryArray[5] ^ binaryArray[3] ^ binaryArray[1]))
              && (binaryArray[14] == !(binaryArray[12] ^ binaryArray[10] ^ binaryArray[8] ^ binaryArray[6] ^ binaryArray[4] ^ binaryArray[2] ^ binaryArray[0])))
        {
          //we got back a good position, so just mask away the checkbits
          currentPosition &= 0x3FFF;
        }
      else
      {
        currentPosition = 0xFFFF; //bad position
      }

      //If the resolution is 12-bits, and wasn't 0xFFFF, then shift position, otherwise do nothing
      if ((resolution == RES12) && (currentPosition != 0xFFFF)) currentPosition = currentPosition >> 2;

      return currentPosition;
    }
    
    
    void setZeroSPI(uint8_t encoder)
    {
      spiWriteRead(AMT22_NOP, encoder, false);

      //this is the time required between bytes as specified in the datasheet.
      //We will implement that time delay here, however the arduino is not the fastest device so the delay
      //is likely inherantly there already
      delayMicroseconds(3); 
      
      spiWriteRead(AMT22_ZERO, encoder, true);
      delay(250); //250 second delay to allow the encoder to reset
    }
  //functiuon to stop all motors
  void stop(){
    digitalWrite(ascHbridge1, LOW);
    digitalWrite(ascHbridge2, LOW);
    digitalWrite(decHbridge1, LOW);
    digitalWrite(decHbridge2, LOW);
  }

void loop() {
  //vars for loop
  int ascLim_r = digitalRead(asclim1);
  int ascLim_r1 = digitalRead(asclim2);
  int decLim_r = digitalRead(declim1);
  //bool error = false;
  float currentAsc = 0;
  float currentDec = 0;
  //run logic
    if (digitalRead(estop) == HIGH){
        digitalWrite(estopLed, HIGH);
    }
    if (ascLim_r == 1){
      Serial.write("ascL1\n");
      stop();
    }
    if (ascLim_r1 == 1){
      Serial.write("ascL2\n");
      stop();
    }
    if (decLim_r = 1){
      Serial.write("decL1\n");
      stop();
    }
  //serial to micro {telescope_pendant}
  
  //encoder stuff
    //create a 16 bit variable to hold the encoders position
    uint16_t encoderPosition;
    //let's also create a variable where we can count how many times we've tried to obtain the position in case there are errors
    uint8_t attempts;


    //if you want to set the zero position before beggining uncomment the following function call
    //setZeroSPI(ENC_0);
    //setZeroSPI(ENC_1);

   
      //set attemps counter at 0 so we can try again if we get bad position    
      attempts = 0;

      //this function gets the encoder position and returns it as a uint16_t
      //send the function either res12 or res14 for your encoders resolution
      encoderPosition = getPositionSPI(ENC_0, RES14); 

      //if the position returned was 0xFFFF we know that there was an error calculating the checksum
      //make 3 attempts for position. we will pre-increment attempts because we'll use the number later and want an accurate count
      while (encoderPosition == 0xFFFF && ++attempts < 3) {
        encoderPosition = getPositionSPI(ENC_0, RES14); //try again
      }

      if (encoderPosition == 0xFFFF) { //position is bad, let the user know how many times we tried
        Serial.print("Encoder 0 error. Attempts: ");
        Serial.print(attempts, DEC); //print out the number in decimal format. attempts - 1 is used since we post incremented the loop
        Serial.write(NEWLINE);
      }
      else { //position was good, print to serial stream
        
        Serial.print("Encoder 0: ");
        Serial.print(encoderPosition, DEC); //print the position in decimal format
        Serial.write(NEWLINE);
      }

      //////////again for second encoder//////////////////////////////
      
      //set attemps counter at 0 so we can try again if we get bad position    
      attempts = 0;

      //this function gets the encoder position and returns it as a uint16_t
      //send the function either res12 or res14 for your encoders resolution
      encoderPosition = getPositionSPI(ENC_1, RES14); 

      //if the position returned was 0xFFFF we know that there was an error calculating the checksum
      //make 3 attempts for position. we will pre-increment attempts because we'll use the number later and want an accurate count
      while (encoderPosition == 0xFFFF && ++attempts < 3){
        encoderPosition = getPositionSPI(ENC_1, RES14); //try again
      }

      if (encoderPosition == 0xFFFF){ //position is bad, let the user know how many times we tried
        Serial.print("Encoder 1 error. Attempts: ");
        Serial.print(attempts, DEC); //print out the number in decimal format. attempts - 1 is used since we post incremented the loop
        Serial.write(NEWLINE);
      }
      else{ //position was good, print to serial stream
        
        Serial.print("Encoder 1: ");
        Serial.print(encoderPosition, DEC); //print the position in decimal format
        Serial.write(NEWLINE);
      }

      //For the purpose of this demo we don't need the position returned that quickly so let's wait a half second between reads
      //delay() is in milliseconds
      delay(500);
  // end of the enoder 
   } //end loop

void resetAMT22(uint8_t encoder){
      spiWriteRead(AMT22_NOP, encoder, false);

      //this is the time required between bytes as specified in the datasheet.
      //We will implement that time delay here, however the arduino is not the fastest device so the delay
      //is likely inherantly there already
      delayMicroseconds(3); 
      
      spiWriteRead(AMT22_RESET, encoder, true);
      
      delay(250); //250 second delay to allow the encoder to start back up
}
