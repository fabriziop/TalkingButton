=============================
TalkingButton Arduino Library
=============================

TalkingButton library transforms a press button with a single contact into a
powerful input device for any Arduino hardware. It is an easy way to go
beyond the on/off capability of a press button, allowing it to input
a number, a code or a sequence of them. This can be done by chosing
among different button press modes.


Features
========

* Count press mode: inputs the times the button was pressed.
* Binary press mode: short button press input 0, long button press input 1.
* Duration press mode: input the duration of each button press.
* Button contact debounce
* Button contact noise detection
* Very simple API (3 required methods)
* No multithread needed
* No interrupts needed
* No external dependencies
* Supports AVR328p, ESP8266, ESP32 processors.
* Tested on arduino nano, nodeMCU 8266, nodeMCU 32S boards.


Quick start
===========

Here is a simple example of an Arduino application using TalkingButton.
The source file is in examples/press_count.
This program shows the count mode of the TalkingButton library. In this
mode, the library returns the times the button was pressed. It is the
simplest way to use a press button as input device. The count sequence
is terminated by a button release lasting 0.5s at least. When the press
sequence is terminated, the on board LED is flashed the same number of
times the button was pressed. The number of times is also printed to the
arduino serial line.

This program needs a press button whose contact is connected to the 
pin BUTTON_PIN (pin n. 5) of an arduino board on one side and to ground of the
same board on the other side.

The program expects also to have the onboard LED connected to pin n. 13,
to have the LED lighted on when its pin is into high state and to have the LED
lighted off when its pin is into low state.

.. code:: cpp

  #define BUTTON_PIN 5   // assume button contact connected to pin 5
  #define LED_PIN 13     // assume on board LED connected to pin 13
  #define LED_ON HIGH    // assume LED lighted on when LED pin is HIGH
  #define LED_OFF LOW    // assume LED lighted off when LED pin is LOW

  #include <TalkingButton.h>

  TalkingButton TB;


  void setup() {

    Serial.begin(9600);
  
    // init talking button pin and press mode to COUNT mode
    TB.begin(BUTTON_PIN,TB.COUNT);

    Serial.println("Example: press count");

  }


  void loop() {

    // sample button status at least every MAX_READ_PERIOD milliseconds
    // (default 20 ms)
    if (!TB.read())
      Serial.println(TB.strError());
  
    // if button press sequence is terminated, appens when there is a button
    // release lasting at least MESSAGE_SEPARATOR milliseconds (default 500 ms)
    uint8_t count;
    if (TB.readMessage(&count)) {

      // print out the number of button press times
      Serial.print("count = ");
      Serial.println(count);

      // blink on board LED the number of button press times
      for (unit8_t i=0; i < count; i++) {
        digitalWrite(LED_PIN,LED_ON);
        delay(250);
        digitalWrite(LED_PIN,LED_OFF);
        delay(250);
      }

      // flush "read period too long" error, needed since the LED blinking
      // exceeds MAX_READ_PERIOD (default 20 ms)
      TB.read();

    }

    delay(1);

  }

The parameters MAX_READ_PERIOD a MESSAGE_SEPARATOR mentioned above, can be
adjusted from their default values with **setMaxReadPeriod** and
**setMessageSeparator** methods respectively.


Button press modes
==================

TalkingButton implements 3 button press modes: **COUNT**, **BINARY**,
**DURATION**. A press mode establish how a sequence of button presses
is converted by TalkingButton into a code/message for the application.

**COUNT** mode
--------------

This is the simplest input mode within everyone's reach. In this mode,
the button presses are counted and when the press sequence terminate,
this count is available for reading by the application. A press sequence
can start only after a button release state lasting at least MESSAGE_SEPARATOR
milliseconds (default 500 ms). The sequence is ended by another message
separator. The MESSAGE_SEPARATOR value can be adjusted with the
**setMessageSeparator** method.

The button press/release timing is quit elastic in the **COUNT** mode. There
is no upper limit on the duration off each press. In addition, each
duration can be different from the others and the same degrees of fredoom apply
to button releases. The only limit is the minimum duration of both press
and release status (default 100 ms). These minimum durations can be
respectively adjusted with the **setMinPress** and **setMinRelease** methods.

**BINARY** mode
---------------

This mode requires a little bit of morse operator skill. In this mode,
a sequence of button presses encodes a binary number. A short press
represent a zero bit, a long press represent a one bit. Long and short
press durations are discriminated by the threshold value
BINARY_HIGH_LOW_THRESHOLD (default 300 ms) that can be adjusted with the
**setBinaryHighLowThreshold** method. The number of bits is given by the
number of presses in the sequence. If the bit number exceeds the value
MAX_BIT_NUM the **read** method returns an error, the input
sequence decoding is reset and a new sequence in needed. The sequence
start and stop follows the same rules of **COUNT** mode. The first bit
encoded is the nost significant bit. The last one is the least significant
bit.

**DURATION** mode
-----------------

This mode has an intermediate difficulty level between **COUNT** and
**BINARY** modes. In this mode, a sequence of button presses is decoded
as a sequence of numbers of the same lenght where each number is the
duration of the corresponding button press measured as DURATION_UNIT
milliseconds (default 1000 ms), this parameter can be adjusted with the
**setDurationUnit** method. If the sequence lenght exceeds the value
MAX_DIGIT_NUM (default 4), the **read** method returns an error, the input
sequence decoding is reset and a new sequence in needed.
The sequence start and stop follows the same rules of **COUNT** mode.


Examples
========

See the "examples" directory.


Installing
==========

By arduino IDE library manager or by unzipping TalkingButton.zip into
arduino libraries.


Module reference
================

TalkingButton is implemented as a C++ class. A TalkingButton object needs to be
instantiated and associated to the Arduino pin connected to the press button
to be managed.


Objects and methods
-------------------

**TalkingButton**

  This class embeds all TalkingButton status info.


bool **begin(** uint8_t **aButtonPin**,
  enum pressMode **aPressMode** = COUNT,bool **aPullup** = true,
  bool **aPressedLow** = false)

  This method init TalkingButton internals setting the given parameters or
  their default values.

  **aButtonPin**: the number of pin connected to the press button contact.

  **aPressMode**: how button presses are interpreted.
  COUNT: count the number of button presses.
  BINARY: short button press encodes 0, long  encodes 1.
  DURATION: duration of each button press in duration units
  (default seconds). More details in the documentation.

  **aPullup**: tell if pullup is to be activated on the button pin.

  **aPressedLow**: tell if the input level on the button pin is low when
  the button is pressed (=true) or high (=false).

  Returns **true** .
 

bool **read()**

  This method read the button contact input pin and sample its status.
  It implements all the internal login to interpret the button presses
  according to the selected press mode: count, binary or duration.
  
  Returns **true** if there is no error. Returns **false** if an error
  occurred. In this case, the error kind can be determined reading the
  code in the **error** attribute of TalkingButton class.


bool **readMessage(** uint8_t * **aMessage)**

  This method read the message input by button presses according to the
  selected press mode. Any button sequence of presses/releases preceeded
  and followed by a message separator, a button release lasting at least
  **MESSAGE_SEPARATOR** milliseconds, is defined a **message**. Its
  meaning depends on the selected press mode.

  **aMessage**: where to return the message value. If button press
  mode is **COUNT**, it is a single uint8_t value counting the times the
  button was pressed. If mode is **BINARY**, it is a single uint8_t value
  as encoded by button presses: short press for zeros, long press for ones.
  If mode is **DURATION**, it is an array of one or more uint8_t values,
  each value is the press duration in duration units (default 1 s). The
  sequence is terminated by a zero value.

  Returns **true** if there is a message ready to be read. Returns 
  **false** if there is no ready message. Any read after the first one
  on a ready message returns **false**.


bool **setMaxReadPeriod(** uint8_t **aMaxReadPeriod)**

  This method allows to set the value of the **maxReadPeriod** parameter.

  **aMaxReadPeriod**: it is the maximum allowed elapsed time (ms) between two
  consecutive calls to read method (default 20 ms). If not satisfied,
  **read** method returns **false** and error code **READ_PERIOD_TOO_LONG**,
  but message decoding continues keeping previous button presses/releases.

  Returns always **true**.


bool **setMinReleased(** uint16_t **aMinReleased)**

  This method allows to set the value of the **minReleased** parameter.

  **aMinReleased**: it is the minimum allowed duration (ms) of a button
  release. If not satisfied, message decoding is reset to an initial state,
  so a new message separator it is needed to start a new decoding of button
  presses/releases (default 100 ms).

  Returns always **true**.


bool **setMinPressed(** uint16_t **aMinPressed)**

  This method allows to set the value of the **minPressed** parameter.

  **aMinPressed**: it is the minimum allowed duration (ms) of a button press.
  If not satisfied, see method **setMinReleased** (default 100 ms).

  Returns always **true**.


bool **setMessageSeparator(** uint16_t **aMessageSeparator)**

  This method allows to set the value of the **messageSeparator** parameter.

  **aMessageSeparator**: it is the minimum duration (ms) of a button release
  that separates a message (see **readMessage**) from the following one
  (default 500 ms).

  Returns always **true**.


bool **setMaxCount(** uint8_t **aMaxCount)**

  This method allows to set the value of the **maxCount** parameter.

  **aMaxCount**: it is the maximum allowed value for a message in **COUNT**
  press mode (default 10). If not satisfied, see method **setMinReleased**
  (default 100 ms).
      
  Returns always **true**.


bool **setMaxBitNum(** uint8_t **aMaxBitNum)**

  This method allows to set the value of the **maxBitNum** parameter.

  **aMaxBitNum**: it is the maximum allowed number of bits to be encoded
  from button presses in **BINARY** press mode (default 4).
      
  Returns always **true**.


bool **setBinaryHighLowThreshold(** uint16_t **aBinaryHighLowThreshold)**

  This method allows to set the value of the **binaryHighLowThreshold**
  parameter.

  **aBinaryHighLoeThreshold**: the threshold level that discriminate a
  button press duration to be a zero or a one in **BINARY** press mode
  (default 300 ms). Duration above this threshold are taken as ones,
  below as zeros.
      
  Returns always **true**.


bool **setMaxDigitNum(** uint8_t **aMaxDigitNum)**

  This method allows to set the value of the **maxDigitNum** parameter.

  **aMaxDigitNum**: the maximum allowed number of digits to be encoded
  from button presses in **DURATION** press mode (default 4). This is
  also the maximum number of button presses withing a message.
      
  Returns always **true**.


bool **setDurationUnit(** uint8_t **aDurationUnit)**

  This method allows to set the value of the **durationUnits** parameter.

  **durationUnits**: time units in milliseconds to measure the button presses
  duration in **DURATION** press mode (default 1000 ms).
      
  Returns always **true**.


char * **strError(** void);

  This method returns a string describing the current error code.

    ============================ ============================================
    Error Codes and error description
    -------------------------------------------------------------------------
    Code                         Description
    ============================ ============================================
    SUCCESS                      "no error"
    READ_PERIOD_TOO_LONG         "button read period is too long"
    READ_NOISE                   "button status changes too fast, read noise"
    RELEASE_TOO_SHORT            "button release is too short"
    PRESS_TOO_SHORT              "button press is too short"
    COUNT_OVERFLOW               "press count too high in count mode"
    BINARY_BITS_NUM_OVERFLOW     "too many bits read in binary mode"
    DURATION_DIGITS_NUM_OVERFLOW "too many digits read in duration mode"
    ============================ ============================================
 
  Returns a pointer to the string describing the current error code.
 

Internals
=========

Collection of rules and assumptions implemented in this software.

+ Rule: at message start, since it is represented by a button release
  status, the first status change starts a pressed status. A second
  change closes the pressed status. The first change must be ignored by
  message parsing, since only the following changes are relevant to 
  the message content until the next message separator that terminates
  the message.

+ Rule: status duration is the time interval between first read and last read
  unchanged. The read where the status changes it is not taken into
  account for duration computation.

+ Assumption: it is supposed that the message separator time interval is much
  more longer the the maximum read time interval, more than 10 times.

+ Rule: message separator can be detected only when status is unchanged. Since
  only this situation increments the currentStatusDuration which value
  can reach the "message separator" time interval amplitude.
  
+ Message parsing statuses

  - INIT: the very beginning after boot/reset
  - WAITING FOR MESSAGE: just after detection of a message separator
    (a released status lasting at least MESSAGE_SEPARATOR value) while in
    "INIT" status
  - IN MESSAGE: just after a change in button status while in "WFM" status
  - END OF MESSAGE: just after a message separator while in "IN MESSAGE" status


Contributing
============

Send wishes, comments, patches, etc. to mxgbot_a_t_gmail.com .


Copyright
=========

TalkingButton is authored by Fabrizio Pollastri <mxgbot_a_t_gmail.com>,
years 2021-2022, under the GNU Lesser General Public License version 3.

.. ==== END
