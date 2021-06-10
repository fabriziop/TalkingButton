/* .+

.context    : Arduino Utility Libraries
.title      : test the binary mode of TalkingButton Library
.kind       : c++ source
.author     : Fabrizio Pollastri <mxgbot@gmail.com>
.site       : Revello - Italy
.creation   : 26-Apr-2021
.copyright  : (c) 2021 Fabrizio Pollastri
.description
  This program tests the binary mode of the TalkingButton library. In this
  mode, the library returns the binary number encoded by button pressing.
  Short press for zeros and long press for ones. This program is endless
  cycling. At each cycle, a random number between 1 and 255 is computed,
  the same number is binary encoded with button pressing that is read and
  decoded by the library. The read code is compared for equality with the
  original random value.
  Test result are printed out to the arduino serial line.
.note
  This program needs a jumper between th arduino pins BUTTON_PIN and
  BUTTON_PIN_DRIVER.
  
.- */

#ifdef __AVR__
  #define BUTTON_PIN 2
  #define BUTTON_PIN_DRIVER 3
#elif ESP8266
  #define BUTTON_PIN D2
  #define BUTTON_PIN_DRIVER D3
#else
  #error TalkingButton: unsuported processor.
#endif

#include <stdio.h>
#include <TalkingButton.h>

TalkingButton TB;


void pressRelease(TalkingButton &TB,int pressTime,int releaseTime) {

  uint32_t start;
 
  // press
  if (pressTime) {
    digitalWrite(BUTTON_PIN_DRIVER,LOW);
    start = millis();
    while (millis() - start <= pressTime) {
      if (!TB.read()) {
        Serial.print("p");
        Serial.println(TB.error);
      }
      delay(2);
    }
  }

  // release
  if (releaseTime) {
    digitalWrite(BUTTON_PIN_DRIVER,HIGH);
    start = millis();
    while (millis() - start <= releaseTime) {
      if (!TB.read()) {
        Serial.print("r");
        Serial.println(TB.error);
      }
      delay(2);
    }
  }

}


void setup() {

  Serial.begin(9600);
  
  // init button pin driver (high -> released, low -> pressed)
  pinMode(BUTTON_PIN_DRIVER,OUTPUT);
  digitalWrite(BUTTON_PIN_DRIVER,HIGH);

  // init talking button pin and press mode
  TB.begin(BUTTON_PIN,TB.BINARY);

  Serial.println("test 'binary' press mode");

  // go to wait for message status
  pressRelease(TB,0,TB.MESSAGE_SEPARATOR + 20);
  
}


void loop() {

  bool isTestError = false;

  // get a random number of button press
  uint8_t code = random(0,16);
  uint8_t msg = code;
  Serial.print("code = ");
  Serial.println(code);

  // do button press/release for the given count
  for (int n=0; n < 4; n++) {
    if (msg & 0x8)
      pressRelease(TB,500,TB.MIN_RELEASED + 20);
    else
      pressRelease(TB,120,TB.MIN_RELEASED + 20);
    msg <<= 1;
  }
    
  // go to wait for message status
  pressRelease(TB,0,TB.MESSAGE_SEPARATOR + 20);
     
  // compare given count with read count
  uint8_t message;
  if (!TB.readMessage(&message)) {
    Serial.println("Message unavailable");
    isTestError = true;
  }
  else {
    if (code != message) {
      Serial.print("Read wrong message: expected ");
      Serial.print(code);
      Serial.print(" , read ");
      Serial.println(message);
      isTestError = true;
    }
  }
 
  if (!isTestError)
    Serial.println("TEST OK");

}

/**** end ****/
