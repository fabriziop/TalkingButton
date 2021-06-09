/* .+

.context    : Arduino Utility Libraries
.title      : show count mode of TalkingButton Library
.kind       : c++ source
.author     : Fabrizio Pollastri <mxgbot@gmail.com>
.site       : Revello - Italy
.creation   : 1-May-2021
.copyright  : (c) 2021 Fabrizio Pollastri
.description
  This program shows the count mode of the TalkingButton library. In this
  mode, the library returns the number of button presses. The count sequence
  is terminated by a button release lasting 0.5s at least. When the press
  sequence is terminated, the on board LED is blinked the same number of
  times the button was pressed. The number of times is also printed to the
  arduino serial line. Wait for LED blinking to finish before the beginning
  of another button press sequence.
.note
  This program needs a press button whose contact is connected to the arduino
  pin BUTTON_PIN on one side and to ground on the other side.
  
.- */

#define BUTTON_PIN 5
#ifdef __AVR__
  #define LED_ON HIGH
  #define LED_OFF LOW
  #define LED_PIN 13
#elif ESP8266
  #define LED_ON LOW
  #define LED_OFF HIGH
  #define LED_PIN 2
#else
  #error unsupported processor.
#endif

#include <TalkingButton.h>

TalkingButton TB;


void setup() {

  Serial.begin(9600);
  
  // init talking button pin and press mode
  TB.begin(BUTTON_PIN,TB.COUNT);

  // set LED pin mode
  digitalWrite(LED_PIN,LED_OFF);
  pinMode(LED_PIN,OUTPUT);

  Serial.println("Example: press count");

}


void loop() {

  // sample button status
  if (!TB.read())
    Serial.println(TB.strError());
  
  // if count sequence is terminated ...
  uint8_t count;
  if (TB.readMessage(&count)) {

    Serial.print("count = ");
    Serial.println(count);

    // blink on board LED count times
    for (uint8_t i=0; i < count; i++) {
      digitalWrite(LED_PIN,LED_ON);
      delay(250);
      digitalWrite(LED_PIN,LED_OFF);
      delay(250);
    }

    // flush "read period too long" error
    TB.read();

  }

  delay(1);

}

/**** end ****/
