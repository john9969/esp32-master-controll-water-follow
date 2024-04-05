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
    void setFrequency(const uint8_t & free){
        this->frequencyPerHour = free;
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
    void calculaRemainTime(){
        uint32_t secondLeft = 0;
        if(nextTime.Minute == currentTime.Minute){
            this->_secondLeft =0;
            return;
        }
        if(nextTime.Minute > currentTime.Minute){
            secondLeft = (nextTime.Minute- currentTime.Minute); 
        }
        else {
            secondLeft = nextTime.Minute +60 - currentTime.Minute;
        }
        secondLeft *= 60;
        this->_secondLeft = secondLeft+ (frequencyPerHour-1) *60;
        Serial.println("next min" + String(this->nextTime.Minute)+ "current: "+ String(currentTime.Minute));
        Serial.println("remain time: " + String(this->_secondLeft));
    }
    void resetAlarm(){
        
        this->isRinging = false;
    }

    void runAlarm() {
        static uint8_t lastSec=0;
        static uint8_t lastRemainTime =0;
        static int coutdownShowLcd = 10;  
        if(isRinging){
            return;
        }
        Rtc* rtc = Rtc::getInstance();
        Lcd* lcd = Lcd::getInstance();
        rtc->getTime(currentTime);
        
        if(currentTime.Hour == 23){
            if(currentTime.Minute == 30 && currentTime.Second ==0){
                ESP.restart();
            }
        }
        // int value = nextTime.Minute - current.Minute;
        // if(lastSec == current.Second) {
        //     return;
        // }
        int delta = currentTime.Second - lastSec;
        if(delta == 0) return;
        Serial.println("alarm init currentsec: "+ String(currentTime.Second)+ "last: "+ String(lastSec));
        calculaRemainTime();
        updateRemainTime();
        if(this->_secondLeft <= 0){
            if(!isRinging)
                isRinging = true;
        }
        
        lastSec = currentTime.Second;
    }
};
Alarm* Alarm::_alarm= nullptr;