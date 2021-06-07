/* .+

.context    : Arduino Utility Libraries
.title      : TalkingButton Library
.kind       : c++ source
.author     : Fabrizio Pollastri <mxgbot@gmail.com>
.site       : Revello - Italy
.creation   : 28-Feb-2021
.copyright  : (c) 2021 Fabrizio Pollastri
.license    : GNU Lesser General Public License
.description
  TalkingButton library transforms a press button with a single contact
  into a powerful input device for any Arduino hardware. It is an easy way
  to go beyond the on/off capability of a press button, allowing it to input
  a number, a code or a sequence of them. This can be done by chosing
  among different button press modes.

.- */

#include "TalkingButton.h"


TalkingButton::TalkingButton(void) {

  // init parameters with default values
  setMaxReadPeriod(MAX_READ_PERIOD);
  setMinReleased(MIN_RELEASED);
  setMinPressed(MIN_PRESSED);
  setMessageSeparator(MESSAGE_SEPARATOR);
  setMaxCount(MAX_COUNT);
  setMaxBitNum(MAX_BIT_NUM);
  setBinaryHighLowThreshold(BINARY_HIGH_LOW_THRESHOLD);
  setMaxDigitNum(MAX_DIGIT_NUM);
  setDurationUnit(DURATION_UNIT);

  // init internal variables not time sensitive
  currentStatusDuration = 0;
  error = SUCCESS;
  parseStatus = INIT;
 
}


bool TalkingButton::begin(uint8_t aButtonPin,
    enum pressMode aPressMode,bool aPullup,bool aPressedLow) {

  // set user/default parameters
  buttonPin = aButtonPin;
  pressMode = aPressMode;
  pinMode(buttonPin,aPullup ? INPUT_PULLUP : INPUT);

  // set dependent variables
  pressed = (uint8_t)!aPressedLow;
  released = (uint8_t)aPressedLow;
  lastStatus = released;

  // init internal variables time sensitive
  lastChange = millis();
  lastReadTime = lastChange;

  return true;

}


bool TalkingButton::read(void) {

  // if button reading speed is too slow, return an error
  unsigned long now = millis();

  if (now - lastReadTime >= maxReadPeriod) {
    lastReadTime = now;
    return _error(READ_PERIOD_TOO_LONG,false);
  }
  lastReadTime = now;

  // read button status
  currentStatus = digitalRead(buttonPin);

  // if no button status change ...
  if (currentStatus == lastStatus) {

    // update duration
    currentStatusDuration = now - lastChange;

    // if it is a message separator ...
    if (currentStatus == released) {
      if (currentStatusDuration >= messageSeparator) {
        switch (parseStatus) {
	  case INIT:
	    parseStatus = WAIT_FOR_MESSAGE;
	    return true;
	  case IN_MESSAGE:
            parseStatus = END_OF_MESSAGE;
            return _parseMessage();
        }
      }
    }
    return true;
  }

  // button status changed ...

  // duration of the last change of button status
  changeAverage += now - lastChange - (changeAverage >> 4);
  lastChange = now;
  status = lastStatus;
  statusDuration = currentStatusDuration;
  lastStatus = currentStatus;
  currentStatusDuration = 0;

  // skip parse of the first button status change after message separator
  if (parseStatus == WAIT_FOR_MESSAGE) {
    parseStatus = IN_MESSAGE;
    return true;
  }

  // debounce last change
  if (statusDuration >= debounceInterval) {

    /* status duration is ok, parse last status to extract the message */
    return _parseMessage();
  }
  else {

    // if status is changing too fast, it is noise, return an error
    if (changeAverage <= minChangeAverage)
      return _error(READ_NOISE);
  }

  return true;

}


bool TalkingButton::_parseMessage() {

  // if receiving message, parse changes timing to decode message 
  // according to press mode
  if (parseStatus == IN_MESSAGE) {

    // if release completed ...
    if (status == released) {

      // If release lasted too short, return error.
      if (statusDuration < minReleased)
	return _error(RELEASE_TOO_SHORT);
    }

    // If press lasted too short, return error.
    else {

      if (statusDuration < minPressed)
	return _error(PRESS_TOO_SHORT);
    }
  }

  // if pressed end or end of message: parse it according to press mode
  if (parseStatus == IN_MESSAGE and status == pressed
    or parseStatus == END_OF_MESSAGE) {
    switch (pressMode) {
      case COUNT:
        return _parseCount();
      case BINARY:
        return _parseBinary();
      case DURATION:
        return _parseDuration();
    }
  }

  return true;

}


bool TalkingButton::_parseCount() {

  if (parseStatus == END_OF_MESSAGE) {
    if (count) {
      message[0] = count;
      count = 0;
      error = SUCCESS;
      isMessageAvailable = true;
    }
    parseStatus = WAIT_FOR_MESSAGE;
    return true;
  }

  count++;

  if (count > maxCount)
    return _error(COUNT_OVERFLOW);

  return true;

}


bool TalkingButton::_parseBinary() {

  if (parseStatus == END_OF_MESSAGE) {
    if (bitNum) {
      message[0] = binaryCode;
      binaryCode = 0;
      bitNum = 0;
      error = SUCCESS;
      isMessageAvailable = true;
    }
    parseStatus = WAIT_FOR_MESSAGE;
    return true;
  }

  bitNum++;
  if (bitNum > maxBitNum)
    return _error(BINARY_BITS_NUM_OVERFLOW);

  binaryCode <<= 1;
  if (statusDuration > binaryHighLowThreshold)
    binaryCode |= 0x1;

  return true;

}


bool TalkingButton::_parseDuration() {

  if (parseStatus == END_OF_MESSAGE) {
    if (digitNum) {
      message[digitNum] = 0x0;
      digitNum = 0;
      error = SUCCESS;
      isMessageAvailable = true;
    }
    parseStatus = WAIT_FOR_MESSAGE;
    return true;
  }

  if (digitNum > maxDigitNum - 1)
    return _error(DURATION_DIGITS_NUM_OVERFLOW);

  message[digitNum] = (statusDuration + (durationUnit >> 1)) / durationUnit;
  if(!message[digitNum])
    message[digitNum]++;
  digitNum++;

  return true;

}


bool TalkingButton::_error(enum errorCode errorCode,bool reset) {

  error = errorCode;

  if (!reset)
    return false;

  switch (pressMode) {
    case COUNT:
      count = 0;
      break;
    case BINARY:
      binaryCode = 0;
      bitNum = 0;
      break;
    case DURATION:
      digitNum = 0;
  }

  parseStatus = INIT;
  return false;

}


bool TalkingButton::readMessage(uint8_t *aMessage) {

  if (isMessageAvailable) {
    if (pressMode == DURATION) {
      int i = 0;
      while (message[i]) {
        aMessage[i] = message[i];
        i++;
      }
      aMessage[i] = 0x0;
    }
    else
      *aMessage = message[0];
    
    isMessageAvailable = false;
    return true;
  }
   
  return false;

}


bool TalkingButton::setMaxReadPeriod(uint8_t aMaxReadPeriod) {

  // time interval between two consecutive reads: must be less that this value.
  maxReadPeriod = aMaxReadPeriod;
  // noise threshold: button changes shorter than this value is taken as noise.
  // x16: it works as exponential average accumulator.
  // maxReadPeriod * 2 + 2: no noise implies at least 2 consecutive reads with
  // the same value.
  minChangeAverage = (maxReadPeriod << 1) + 2 << 4;
  // preset exponential average accumulator
  changeAverage = minChangeAverage;
  // debounced value lasts at least 2 consecutive reads.
  debounceInterval = (maxReadPeriod << 1) + 2;

  return true;

}


bool TalkingButton::setMinReleased(uint16_t aMinReleased) {

  // min duration allowed for button released state
  minReleased = aMinReleased;

  return true;

}


bool TalkingButton::setMinPressed(uint16_t aMinPressed) {

  // min duration allowed for button pressed state
  minPressed = aMinPressed;

  return true;

}


bool TalkingButton::setMessageSeparator(uint16_t aMessageSeparator) {

  // duration of message separator, released state between messages
  messageSeparator = aMessageSeparator;

  return true;

}


bool TalkingButton::setMaxCount(uint8_t aMaxCount) {

  // count mode: maximum allowed count value
  maxCount = aMaxCount;

  return true;

}


bool TalkingButton::setMaxBitNum(uint8_t aMaxBitNum) {

  // binary mode: maximum allowed number of bits
  maxBitNum = aMaxBitNum;

  return true;

}


bool TalkingButton::setBinaryHighLowThreshold(uint16_t aBinaryHighLowThreshold) {

  // binary mode: press duration threshold between high and low status
  binaryHighLowThreshold = aBinaryHighLowThreshold;

  return true;

}


bool TalkingButton::setMaxDigitNum(uint8_t aMaxDigitNum) {

  // duration mode: maximum allowed number of digits.
  maxDigitNum = aMaxDigitNum;

  return true;

}


bool TalkingButton::setDurationUnit(uint16_t aDurationUnit) {

  // minimum duration increment quantity
  durationUnit = aDurationUnit;

  return true;

}


char *TalkingButton::strError(void) {

  return errorMessages[error];

}


/**** END ****/
