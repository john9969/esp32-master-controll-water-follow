#include <Arduino.h>
#include "board/rtc.h"
#include "config/Config.h"
#include <iostream>
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
    bool isRinging;

    int _minuteLeft;
    int _secondLeft;
    uint32_t totalSecond;
    Time currentTime;
    bool isDay(){
        if(_nightTime > _dayTime)
            return (currentTime.Hour > _dayTime && currentTime.Hour < _nightTime) ? true:false;
        else
            return (currentTime.Hour > _nightTime && currentTime.Hour < _dayTime) ? false: true;
    }
public:
    Alarm(DataConfig* dataConfig) : _dataConfig(dataConfig),_freDay(_dataConfig->_freDay),_freNight(_dataConfig->_freNight),_dayTime(_dataConfig->_dayTime),_nightTime(_dataConfig->_nightTime),isRinging(false),totalSecond(0) {
        this->_timeSecAlarm =100000;
        _secondLeft = 3600;
        _minuteLeft = 99;
        _minuteStart = 55;
        _dataConfig->append(this);
    }
    void ringAlarm() const {
    }
    
    static Alarm* getInstance() {
        if(!_alarm){
            _alarm = new Alarm(DataConfig::getInstance());
        }
        return _alarm;
    }
    void changeAlarm(const uint8_t& minute = 55) {
        Rtc* rtc = Rtc::getInstance();
        this->_minuteStart = minute;
        if(isDay()){
            this->_timeSecAlarm = (_freDay-1)*3600 + _minuteStart * 60 + rtc->getSecondsPoint() - rtc->getSecond() - rtc->getMinute()*60 ;
        }
        else {
            this->_timeSecAlarm = (_freNight-1)*3600 + _minuteStart * 60 + rtc->getSecondsPoint() - rtc->getSecond() - rtc->getMinute()*60 ;
        }
        Serial.println("next Time location: "+ String(this->_timeSecAlarm) + ",current sec: "+ Rtc::getInstance()->getSecondsPoint());
    }
    void hasChanged() override{
        Serial.println("Alarm->hasSignalChange() has signal from config");
        this->_freDay = _dataConfig->_freDay;
        this->_freNight = _dataConfig->_freNight;
        this->_dayTime = _dataConfig->_dayTime;
        this->_nightTime = _dataConfig->_nightTime;
        changeAlarm(_dataConfig->_minuteStart);
    }
    
    void resetAlarm(){
        Rtc * rtc = Rtc::getInstance();
       if(isDay()){

            this->_timeSecAlarm = (_freDay-1)*3600 + _minuteStart * 60 + rtc->getSecondsPoint() - rtc->getSecond() - rtc->getMinute()*60 ;
        }
        else {
            this->_timeSecAlarm = (_freNight-1)*3600 + _minuteStart * 60 + rtc->getSecondsPoint()  - rtc->getSecond() - rtc->getMinute()*60;
        }
        Serial.println("reset alarm finsh, next Time point: "+ String(this->_timeSecAlarm) + ",current time h/m/s:"+ String(rtc->getHour()) + "/" + String(rtc->getMinute())+"/"+ String(rtc->getSecond()));
    }
    
    int getRemainSecond(){
        this->_secondLeft =  this->_timeSecAlarm - (currentTime.Hour*3600+ currentTime.Minute*60 + currentTime.Second);
        return this->_secondLeft;
    }
    int getRemainMinute() {
        this->_secondLeft =  this->_timeSecAlarm - (currentTime.Hour*3600+ currentTime.Minute*60 + currentTime.Second);
        return this->_secondLeft/60;
    }
    void updateRemainTime(){
        Lcd* lcd = Lcd::getInstance();
        if((this->_secondLeft /60) != this->_minuteLeft){
            this->_minuteLeft  = this->_secondLeft/60;
            Serial.println("Alarm->update remain time() minute left: " + String(this->_minuteLeft));
            lcd->show(String((getRemainMinute() > 99)? 99 : getRemainMinute()),Lcd::TYPE_REMAIN_TIME_ALARM,3);
        }
    }
    bool getIsRinging() const {
        return this->isRinging;
    }

    void checked(){
        this->isRinging = false;
    }

    void runAlarm() {
        static uint8_t lastSec=0;
        static uint8_t lastRemainTime =0;
        static int coutdownShowLcd = 10;

        /** Alarm running*/  
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

        int delta = currentTime.Second - lastSec;
        if(delta == 0) return;
        int remainSecound = getRemainSecond();
        // Serial.println("second left: " + String(remainSecound));
        updateRemainTime();
        if(this->_secondLeft <= 0){
            isRinging = true;
            resetAlarm();
        }
        
        lastSec = currentTime.Second;
    }
};
Alarm* Alarm::_alarm= nullptr;