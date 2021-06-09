/* .+

.context    : Arduino Utility Libraries
.title      : show duration mode of TalkingButton Library
.kind       : c++ source
.author     : Fabrizio Pollastri <mxgbot@gmail.com>
.site       : Revello - Italy
.creation   : 2-May-2021
.copyright  : (c) 2021 Fabrizio Pollastri
.description
  This program shows the duration mode of the TalkingButton library. In this
  mode, the library for each button press, returns a number that is the
  duration in duration units (default 1s) of the button press. The press
  sequence is terminated by a button release lasting 0.5s at least. When the
  press sequence is terminated, the on board LED is blinked to display the
  number sequence. For each number, the LED is set on for a time specified by
  the number in duration units. The number sequence is also printed to the
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

#define DURATION_UNIT 1000


void setup() {

  Serial.begin(9600);
  
  // init talking button pin and press mode
  TB.begin(BUTTON_PIN,TB.DURATION);

  // set LED pin mode
  digitalWrite(LED_PIN,LED_OFF);
  pinMode(LED_PIN,OUTPUT);

  // increase message separator from 500 ms (default) to 1000 ms to be more
  // easy with duration mode
  TB.setMessageSeparator(1000);

  Serial.println("Example: press duration");

}


void loop() {

  // sample button status
  if (!TB.read())
    Serial.println(TB.strError());
  
  // if press sequence is terminated ...
  uint8_t sequence[20];
  if (TB.readMessage(sequence)) {

    if (TB.error)
      Serial.println(TB.strError());
    else {
      Serial.print("sequence =");
      uint8_t i = 0;
      while (sequence[i]) {
        Serial.print(" ");
        Serial.print(sequence[i]);
        i++;
      }
      Serial.println();
    }

    // blink on board LED
    uint8_t i = 0;
    while(sequence[i]) {
      digitalWrite(LED_PIN,LED_ON);
      delay(sequence[i] * DURATION_UNIT);
      digitalWrite(LED_PIN,LED_OFF);
      delay(200);
      i++;
    }

    // flush "read period too long" error
    TB.read();

  }

  delay(1);

}

/**** end ****/
