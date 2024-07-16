#ifndef RTC__H
#define RTC__H
#include <Arduino.h>
#include <iostream>
#include "../lcd/Lcd.h"
#include "../../lib/rtc/DS1307RTC.h"
typedef tmElements_t Time;
class Rtc : public DS1307RTC
{
private:
    Rtc();
    static Rtc* _rtc;
    uint8_t _lastSec;
    uint8_t _lastDay;
    uint64_t _totalSecDay =0;
    Time _current;
public:
    void fromString(String data);
    void run();
    uint64_t getSecondsPoint();
    static Rtc* getInstance();
    void getTime(Time& t);
    uint8_t getSecond();
    uint8_t getMinute();
    uint8_t getHour();
    int8_t toString(String& data);
    void setTime(Time t);
};

#endif