#include <Arduino.h>
#include "board/rtc.h"
#include <iostream>
class Alarm {
private:
    static Alarm* _alarm;
    int remainTime;
    Time nextTime;
    bool isRinging;
    int frequencyPerHour;
    int _secondLeft;
    uint32_t totalSecond;
    Time currentTime;

public:
    Alarm() : frequencyPerHour(1),_secondLeft(frequencyPerHour*60),isRinging(false),totalSecond(0) {
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
        return this->_secondLeft/60;
    }
    void updateRemainTime(){
        Lcd* lcd = Lcd::getInstance();
        if((this->_secondLeft /60) != this->remainTime){
            this->remainTime  = this->_secondLeft/60;
            lcd->show(String((getRemainTime() > 99)? 99 : getRemainTime()),Lcd::TYPE_REMAIN_TIME_ALARM,3);
        }
    }
    bool getIsRinging() const {
        return this->isRinging;
    }
    void resetAlarm(){
        this->isRinging = false;
        this->_secondLeft = frequencyPerHour *60;
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
        if(current.Hour == 23){
            if(current.Minute == 30 && current.Second ==0){
                ESP.restart();
            }
        }
        // int value = nextTime.Minute - current.Minute;
        // if(lastSec == current.Second) {
        //     return;
        // }
        int delta = current.Second - lastSec;
        if(delta == 0) return;
        if(delta != 1 && delta != (-59)) {
            Serial.println("ERR: wrong RTC");
            return;
        }
        if(this->_secondLeft > 0){
            updateRemainTime();
            this->_secondLeft --;
        }
        else{
            if(!isRinging)
                isRinging = true;
        }
        lastSec = current.Second;

        // if(value == 0){
        //     if((current.Second - nextTime.Second) < 5){
        //         isRinging = true;
        //         lcd->show(String(0),Lcd::TYPE_REMAIN_TIME_ALARM,3);
        //     }
        //     else{
        //         remainTime =60;
        //     }   
        // }
        // else if(value > 0)  {
        //     remainTime = value;
        // }
        // else{
        //     remainTime = (value + 60);
        // }

        // if(lastRemainTime != remainTime){
        //     lcd->show(String(remainTime),Lcd::TYPE_REMAIN_TIME_ALARM,3);
        //     lastRemainTime = remainTime;
        // }
        // lastSec = current.Second;
    }
};
Alarm* Alarm::_alarm= nullptr;