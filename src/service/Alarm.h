#include <Arduino.h>
#include "board/rtc.h"
#include <iostream>
class Alarm {
private:
    static Alarm* _alarm;
    int remainTime;
    Time nextTime;
    bool isRinging;
    bool checked;
    int frequencyPerHour;
    uint32_t totalSecond;
    Time currentTime;

public:
    Alarm() : frequencyPerHour(1),isRinging(false),checked(false),totalSecond(0) {
    }
    void ringAlarm() const {
    }
    static Alarm* getInstance() {
        if(!_alarm){
            _alarm = new Alarm();
        }
        return _alarm;
    }

    void setAlarm(const Time& time) {
        nextTime = time;
    }
    void setMinuteAlarm(const uint8_t & minute){
        Time setTime = {0,minute,0,0,0,0,0};
        setAlarm(setTime);
    }
    int getRemainTime() const {
        return this->remainTime;
    }
    bool getIsRinging() const {
        return this->isRinging;
    }
    void runAlarm() {
        static uint8_t lastSec=0;
        static uint8_t lastRemainTime =0;
        static int coutdownShowLcd = 10;  
        if(isRinging){
            return;
        }
        Time current;
        Rtc* rtc = Rtc::getInstance();
        Lcd* lcd = Lcd::getInstance();
        rtc->getTime(current);
        int value = nextTime.Minute - current.Minute;
        if(lastSec == current.Second) {
            return;
        }
        lastSec = current.Second;
        if(value == 0){
            if(nextTime.Second == current.Second){
                isRinging = true;
                lcd->show(String(remainTime),Lcd::TYPE_REMAIN_TIME_ALARM,3);
            }   
        }
        else if(value > 0)  {
            remainTime = value;
        }
        else{
            remainTime = (value + 60);
        }

        if(lastRemainTime != remainTime){
            lcd->show(String(remainTime),Lcd::TYPE_REMAIN_TIME_ALARM,3);
            lastRemainTime = remainTime;
        }
    }
};
Alarm* Alarm::_alarm= nullptr;