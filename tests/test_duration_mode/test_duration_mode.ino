/* .+

.context    : Arduino Utility Libraries
.title      : test the duration mode of TalkingButton Library
.kind       : c++ source
.author     : Fabrizio Pollastri <mxgbot@gmail.com>
.site       : Revello - Italy
.creation   : 27-Apr-2021
.copyright  : (c) 2021 Fabrizio Pollastri
.description
  This program tests the duration mode of the TalkingButton library. In this
  mode, the library returns the pressed status durations of a sequence of button
  press. This program is endless cycling. At each cycle, a sequence of random
  length (from 1 to 4) of pressed status, each one of random duration (from 1
  to 4 duration units, default to 1 second) is feed to button input and read and
  decoded by the library. The read sequence code is compared for equality
  with the original one.
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
  TB.begin(BUTTON_PIN,TB.DURATION);

  Serial.println("test 'duration' press mode");

  // go to wait for message status
  pressRelease(TB,0,TB.MESSAGE_SEPARATOR + 20);
  
}


void loop() {

  bool isTestError = false;

  // random sequence length
  uint8_t seqlen = random(1,5);

  // fill random values into sequence
  uint8_t sequence[4];
  Serial.print("sequence =");
  for (uint8_t i=0; i < seqlen; i++) {
    sequence[i] = random(1,5);
    Serial.print(" ");
    Serial.print(sequence[i],HEX);
  }
  Serial.println();
 
  // do button press/release for the given count
  for (int i=0; i < seqlen; i++) {
    uint16_t pressTime = TB.DURATION_UNIT * sequence[i];
    pressRelease(TB,pressTime,TB.MIN_RELEASED + 20);
  }
    
  // go to wait for message status
  pressRelease(TB,0,TB.MESSAGE_SEPARATOR + 20);
     
  // compare given count with read count
  uint8_t message[5];
  if (!TB.readMessage(message)) {
    Serial.println("Message unavailable");
    isTestError = true;
  }
  else {
    for (uint8_t i=0; i < seqlen; i++) {
      if (sequence[i] != message[i]) {
        Serial.print("Read wrong message: ");
        for (uint8_t j=0; j < seqlen; j++)
          Serial.print(message[j]);
        Serial.println();
        isTestError = true;
        break;
      }
    }
  }

  if (!isTestError)
    Serial.println("TEST OK");

}

/**** end ****/
