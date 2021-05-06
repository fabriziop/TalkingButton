/* .+

.context    : Arduino Utility Libraries
.title      : TalkingButton Library
.kind       : c++ include
.author     : Fabrizio Pollastri <mxgbot@gmail.com>
.site       : Revello - Italy
.creation   : 28-Feb-2021
.copyright  : (c) 2021 Fabrizio Pollastri
.license    : GNU Lesser General Public License

.- */


#ifndef TALKING_BUTTON_H
#define TALKING_BUTTON_H

#include <Arduino.h>


class TalkingButton {

  public:

  enum pressMode {
    COUNT,
    BINARY,
    DURATION
  };

  enum errorCode {
    SUCCESS,
    READ_PERIOD_TOO_LONG,
    READ_NOISE,
    RELEASE_TOO_SHORT,
    PRESS_TOO_SHORT,
    COUNT_OVERFLOW,
    BINARY_BITS_NUM_OVERFLOW,
    DURATION_DIGITS_NUM_OVERFLOW
  };

  // default parameters
  static const uint8_t MAX_READ_PERIOD {20};
  static const uint8_t MAX_COUNT {10};
  static const uint8_t MAX_BIT_NUM {4};
  static const uint8_t MAX_DIGIT_NUM {4};
  static const uint16_t MIN_RELEASED {100};
  static const uint16_t MIN_PRESSED {100};
  static const uint16_t CHANGE_MEAN_PERIOD {50};
  static const uint16_t MESSAGE_SEPARATOR {500};
  static const uint16_t BINARY_HIGH_LOW_THRESHOLD {300};
  static const uint16_t DURATION_UNIT {1000};

  int8_t error;

  // public members
  TalkingButton(void);
  bool begin(uint8_t aButtonPin,enum pressMode pressMode=COUNT,
    bool pullup=true,bool pressedLow=true);
  bool read(void);
  bool readMessage(uint8_t *aMessage);
  bool setMaxReadPeriod(uint8_t aMaxReadPeriod);
  bool setMinReleased(uint16_t aMinReleased);
  bool setMinPressed(uint16_t aMinPressed);
  bool setMessageSeparator(uint16_t aMessageSeparator);
  bool setMaxCount(uint8_t aMaxCount);
  bool setMaxBitNum(uint8_t aMaxBitNum);
  bool setBinaryHighLowThreshold(uint16_t aBinaryHighLowThreshold);
  bool setMaxDigitNum(uint8_t aMaxDigitNum);
  bool setDurationUnit(uint16_t aDurationUnit);
  char *strError(void);
 
  private:

  enum parsingStatus {
    INIT,
    WAIT_FOR_MESSAGE,
    IN_MESSAGE,
    END_OF_MESSAGE
  };

  uint8_t buttonPin;
  uint8_t pressMode;
  uint8_t pressed;
  uint8_t released;
  uint8_t status;
  uint8_t currentStatus;
  uint16_t currentStatusDuration;
  uint8_t lastStatus;
  enum parsingStatus parseStatus;
  uint16_t minReleased;
  uint16_t minPressed;
  uint16_t statusDuration;
  uint32_t lastChange;
  uint32_t changeAverage;
  uint32_t minChangeAverage;
  uint32_t lastReadTime;
  uint8_t maxReadPeriod;
  uint8_t debounceInterval;
  uint16_t messageSeparator;
  uint8_t count {0};
  uint8_t binaryCode {0};
  uint8_t bitNum {0};
  uint8_t digitNum {0};
  uint8_t maxCount;
  uint8_t maxBitNum;
  uint8_t maxDigitNum;
  uint16_t durationUnit;
  uint16_t binaryHighLowThreshold;
  bool isMessageAvailable;
  uint8_t message[16];

  // private members
  bool _parseMessage();
  bool _parseCount();
  bool _parseBinary();
  bool _parseDuration();
  bool _error(enum errorCode errorCode,bool reset=true);

  // error messages
  const char *errorMessages[8] {
    "no error",
    "button read period is too long",
    "button status changes too fast, read noise",
    "button release is too short",
    "button press is too short",
    "press count too high in count mode",
    "too many bits read in binary mode",
    "too many digits read in duration mode" };
 
};

#endif // TALKING_BUTTON_H

/**** END ****/
