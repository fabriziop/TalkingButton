/* .+

.context    : Arduino Utility Libraries
.title      : show binary mode of TalkingButton Library
.kind       : c++ source
.author     : Fabrizio Pollastri <mxgbot@gmail.com>
.site       : Revello - Italy
.creation   : 1-May-2021
.copyright  : (c) 2021 Fabrizio Pollastri
.description
  This program shows the binary mode of the TalkingButton library. In this
  mode, the library returns the binary number encoded by button presses.
  A short press for zeros, a long press for ones. The press sequence
  is terminated by a button release lasting 0.5s at least. When the press
  sequence is terminated, the on board LED is blinked to display the
  binary number, a short blink for zeros, a long blink for ones. The mose
  significant bit is flashed first. The binary number is also printed to the
  arduino serial line. Wait for LED blinking to finish before the beginning
  of another button press sequence.
.note
  This program needs a press button whose contact is connected to the arduino
  pin BUTTON_PIN on one side and to ground on the other side.
  
.- */

#define BUTTON_PIN 5
#define LED_PIN 13
#define LED_ON HIGH
#define LED_OFF LOW

#include <TalkingButton.h>

TalkingButton TB;


void setup() {

  Serial.begin(9600);
  
  // init talking button pin and press mode
  TB.begin(BUTTON_PIN,TB.BINARY);

  // increase message separator from 500 ms (default) to 1000 ms to be more
  // easy with binary mode
  TB.setMessageSeparator(1000);

  Serial.println("Example: press binary");

}


void loop() {

  // sample button status
  if (!TB.read())
    Serial.println(TB.strError());
  
  // if binary sequence is terminated ...
  uint8_t binaryCode;
  if (TB.readMessage(&binaryCode)) {

    Serial.print("binary code = ");
    Serial.println(binaryCode);

    // flash on board LED
    for (uint8_t i=0; i < 4; i++) {
      digitalWrite(LED_PIN,LED_ON);
      if (binaryCode & 0x8)
        delay(300);
      else
        delay(100);
      digitalWrite(LED_PIN,LED_OFF);
      delay(200);
      binaryCode <<= 1;
    }

    // flush "read period too long" error
    TB.read();

  }

  delay(1);

}

/**** end ****/
