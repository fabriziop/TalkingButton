/* .+

.context    : Arduino Utility Libraries
.title      : test the count mode of TalkingButton Library
.kind       : c++ source
.author     : Fabrizio Pollastri <mxgbot@gmail.com>
.site       : Revello - Italy
.creation   : 23-Apr-2021
.copyright  : (c) 2021 Fabrizio Pollastri
.description
  This program tests the count mode of the TalkingButton library. In this
  mode, the library returns the number of times the button was pressed.
  This program is endless cycling. At each cycle, a random number between
  1 and 10 is computed, the same number of time the button is pressed and
  the pressed count read by the library is compared for equality with the
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

  // init talking button pin and push mode
  TB.begin(BUTTON_PIN,TB.COUNT);

  Serial.println("test 'count' press mode");

}


void loop() {

  bool isTestError = false;

  // go to wait for message status
  pressRelease(TB,0,TB.MESSAGE_SEPARATOR + 20);
  
    // get a random number of button press
    int count = random(1,11);
    Serial.print("count = ");
    Serial.println(count);

    // do button press/release for the given count
    for (int n=0; n < count - 1; n++)
      pressRelease(TB,TB.MIN_PRESSED + 20,TB.MIN_RELEASED + 20);
    pressRelease(TB,TB.MIN_PRESSED + 20,TB.MESSAGE_SEPARATOR + 20);
    
    // compare given count with read count
    uint8_t message;
    if (!TB.readMessage(&message)) {
      Serial.print("Message unavailable, error #");
      Serial.println(TB.error);
      isTestError = true;
    }
    else {
          if (count != message) {
        Serial.print("Read wrong message: expected ");
        Serial.print(count);
        Serial.print(" , read ");
        Serial.println(message);
        isTestError = true;
      }
    }
  
  if (!isTestError)
    Serial.println("TEST OK");

}

/**** end ****/
