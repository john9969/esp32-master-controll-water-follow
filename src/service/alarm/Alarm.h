#ifndef ALARM__H
#define ALARM__H
#include <Arduino.h>
#include <iostream>
#include "../../board/rtc/Rtc.h"
#include "../httpRequest/HttpRequest.h"
#include "../config/Config.h"
#define TIME_RESET_MACHINE_POINT 30

class Alarm : public ConfigObserver{
private:
    DataConfig* _dataConfig;
    static Alarm* _alarm;
    uint8_t _dayTime;
    uint8_t _nightTime;
    uint8_t _freDay;
    uint8_t _freNight;
    uint8_t _minuteStart;
    
    int _timeSecAlarm;
    bool _isRinging;
    int _minuteLeft;
    int _secondLeft;
    uint32_t _totalSecond;
    Time _currentTime;
    bool isDay();
public:
    Alarm(DataConfig* dataConfig);
    static Alarm* getInstance();
    void changeAlarm();
    void hasChanged() override;
    void resetAlarm();
    int getRemainSecond();
    int getRemainMinute();
    void updateRemainTime();
    bool getIsRinging() const;
    void checked();
    void runAlarm();
};
#endif // ALARM__H