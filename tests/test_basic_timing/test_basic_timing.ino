/* .+

.context    : Arduino Utility Libraries
.title      : test the basic timing of TalkingButton Library
.kind       : c++ source
.author     : Fabrizio Pollastri <mxgbot@gmail.com>
.site       : Revello - Italy
.creation   : 13-Apr-2021
.copyright  : (c) 2021 Fabrizio Pollastri
.description
  This program tests the basic timing of TalkingButton Library. It tests the
  detection of too low read speed, press/release inverted levels, limits of
  press/release duration, unpressed button behaviour, button contact continuous
  noise and burt noise.
  Test result are printed out to the arduino serial line.
.note
  This program needs a jumper between the arduino pins BUTTON_PIN and
  BUTTON_PIN_DRIVER.
  
.- */

#define BUTTON_PIN 3
#define BUTTON_PIN_DRIVER 4

#define TEST4_DURATION 3000
#define TEST5_DURATION 2000
#define TEST6_DURATION 2000
#define BUTTON_NOISE_PERIOD 6
#define BUTTON_NOISE_CYCLE_PERIOD 2000
#define BUTTON_NORMAL_CYCLE_PERIOD 500

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

}


void loop() {

  Serial.println();
  Serial.println("**** START OF TESTS ****");

  bool isError = false;
  bool isTestError = false;

  //********
  Serial.println("test #1: button maxReadPeriod ckecking.");

  TB.read();  TB.read();  TB.read();
  delay(TB.MAX_READ_PERIOD + 2);

  if (TB.read() || TB.error != TB.READ_PERIOD_TOO_LONG) {
    Serial.println("Checking failed");
    isTestError = true;
  }
  delay(TB.MAX_READ_PERIOD - 2);

  if (!TB.read()) {
    Serial.print("Button read error #");
    Serial.println(TB.error);
    isTestError = true;
  }  
  
  if (isTestError)
    isError = isTestError;
  else
    Serial.println("TEST OK");


  //********
  Serial.println("\ntest #2: check for correct press/release input levels.");
  
  isTestError = false;

  // message run-in: a message separator interval on both press and release
  uint16_t messageSeparator = TB.MESSAGE_SEPARATOR + TB.MAX_READ_PERIOD;
  pressRelease(TB,messageSeparator,messageSeparator);

  // test press/release level inversion
  uint16_t minPressed = TB.MIN_PRESSED - TB.MAX_READ_PERIOD;
  uint16_t minReleased = TB.MIN_RELEASED - TB.MAX_READ_PERIOD;
  pressRelease(TB,minPressed,messageSeparator);

  if (TB.error == TB.RELEASE_TOO_SHORT) {
    pressRelease(TB,messageSeparator,minReleased);
    pressRelease(TB,1,0);

    if (TB.error == TB.PRESS_TOO_SHORT) {
      Serial.println("Press/release levels inverted");
      isTestError = true;
    }
  }
  
  if (isTestError) {
    isError = isTestError;
    Serial.println("TEST FAILED");
  }
  else
    Serial.println("TEST OK");


  //********
  Serial.println("\ntest #3: button press/release min duration detection.");
  
  isTestError = false;

  // message run-in: a message separator.
  pressRelease(TB,0,TB.MESSAGE_SEPARATOR + 20);

  // test min pressed detection
  pressRelease(TB,TB.MIN_PRESSED - 20,TB.MIN_RELEASED + 20);

  if (TB.error != TB.PRESS_TOO_SHORT) {
    Serial.print("Expected error PRESS_TOO_SHORT, instead, read error #");
    Serial.println(TB.error);
    isTestError = true;
  }

  // message run-in: a message separator.
  pressRelease(TB,0,TB.MESSAGE_SEPARATOR + 20);
  
  // test min released detection
  pressRelease(TB,TB.MIN_PRESSED + 20,TB.MIN_RELEASED - 20);
  pressRelease(TB,TB.MIN_PRESSED + 20,0);

  if (TB.error != TB.RELEASE_TOO_SHORT) {
    Serial.print("Expected error RELEASE_TOO_SHORT, instead, read error #");
    Serial.println(TB.error);
    isTestError = true;
  }
  
  if (isTestError) {
    isError = isTestError;
    Serial.println("TEST FAILED");
  }
  else
    Serial.println("TEST OK");


  //********
  Serial.println("\ntest #4: unpressed button correct behaviour.");

  isTestError = false;

  // repeat the test for the requested time
  unsigned long start = millis();

  TB.read();
  while (millis() - start < TEST4_DURATION) {
    
    // check for read errors
    if (!TB.read()) {
      Serial.print("Button read error #");
      Serial.println(TB.error);
      isTestError = true;
    }

    // check for message unavailable
    uint8_t message;
    if (TB.readMessage(message)) {
      Serial.println("readMessage error: message available (should not)");
      isTestError = true;
    }

    delay(1); 
  }

  if (isTestError)
    isError = isTestError;
  else
    Serial.println("TEST OK");
    

  //********
  Serial.println("\ntest #5: button contact noise detection: continuous noise.");
  
  unsigned long now, lastButtonChange;
  long read_ok_count, read_ko_count, read_noise_count;
  uint32_t buttonChangePeriod = BUTTON_NOISE_PERIOD / 2;
   
  isTestError = false;

  // reset button read timings
  TB.read();

  // message run-in: a message separator.
  pressRelease(TB,0,TB.MESSAGE_SEPARATOR + 20);
  
  // repeat the test for the requested time
  start = millis();
  now = start;
  lastButtonChange = start;
  read_ok_count = 0;
  read_ko_count = 0;
  read_noise_count = 0;
  while (now - start < TEST5_DURATION) {
    
    // toggle button at the requested frequency
    if (1) {
    //  if (now - lastButtonChange >= buttonChangePeriod) {
      digitalWrite(BUTTON_PIN_DRIVER,digitalRead(BUTTON_PIN) ^ 0x1);
      lastButtonChange = now;
    }
    delay(1);
    
    // check for read errors
    if (TB.read())
      read_ok_count++;
    else {
      read_ko_count++;
      if (TB.error != TB.READ_NOISE) {
        Serial.print("Unexpected button read error #");
        Serial.println(TB.error);
        isTestError = true;
      }
      else
        read_noise_count++;
    }
    now = millis();
  }
  Serial.print("read ok = "); Serial.println(read_ok_count);
  Serial.print("read ko = "); Serial.println(read_ko_count);
  Serial.print("read noise = "); Serial.println(read_noise_count);

  if (read_noise_count < read_ok_count << 3)
    isTestError = true;
    
  if (isTestError) {
    isError = isTestError;
    Serial.println("TEST FAILED");
  }
  else
    Serial.println("TEST OK");

  //********
  Serial.println("\ntest #6: button contact noise detection: burst noise.");
  
  isTestError = false;

  // reset button read timings
  TB.read();
  
  // repeat the test for the requested time
  start = millis();
  now = start;
  lastButtonChange = start;
  read_ok_count = 0;
  read_ko_count = 0;
  read_noise_count = 0;
  uint32_t burstStart = now;
  uint32_t cycleStart = now;
  uint32_t buttonCyclePeriod = BUTTON_NOISE_CYCLE_PERIOD;
  
  while (now - start < TEST6_DURATION) {

    // alternate cycles of noise bursts and normal press/release
    if (now - cycleStart <= buttonCyclePeriod) {
      // toggle button at the requested frequency
      if (now - lastButtonChange >= buttonChangePeriod) {
        digitalWrite(BUTTON_PIN_DRIVER,digitalRead(BUTTON_PIN) ^ 0x1);
        lastButtonChange = now;
      }
    }
    // set next cycle
    else {
      if (buttonCyclePeriod == BUTTON_NOISE_CYCLE_PERIOD) {
        buttonCyclePeriod = BUTTON_NORMAL_CYCLE_PERIOD;
        buttonChangePeriod = buttonCyclePeriod;
      }
      else {
        buttonCyclePeriod = BUTTON_NOISE_CYCLE_PERIOD;
        buttonChangePeriod = BUTTON_NOISE_PERIOD / 2;
      }
    }
    
    delay(1);
    
    // check for read errors
    if (TB.read())
      read_ok_count++;
    else {
      read_ko_count++;
      if (TB.error != TB.READ_NOISE) {
        Serial.print("Unexpected button read error #");
        Serial.println(TB.error);
        isTestError = true;
      }
      else
        read_noise_count++;
    }
    now = millis();
  }
  Serial.print("read ok = "); Serial.println(read_ok_count);
  Serial.print("read ko = "); Serial.println(read_ko_count);
  Serial.print("read noise = "); Serial.println(read_noise_count);

  if (read_noise_count < read_ok_count >> 1)
    isTestError = true;
    
  if (isTestError) {
    isError = isTestError;
    Serial.println("TEST FAILED");
  }
  else
    Serial.println("TEST OK");

 
 
  // end of tests
  if (isError)
    Serial.println("\nOne o more test FAILED");
  else
    Serial.println("\nALL TESTS OK");
  Serial.println("**** END OF TESTS ****\n");
  
} 

/**** end ****/
