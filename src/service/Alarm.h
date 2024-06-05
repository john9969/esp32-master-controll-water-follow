#include <Arduino.h>
#include "board/rtc.h"
#include "config/Config.h"
#include <iostream>
#define TOTAL_TIME_PER_DAY 86399UL
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
    uint32_t totalSecond;
    Time currentTime;
    bool isDay(){
        if(_nightTime > _dayTime)
            return (currentTime.Hour >= _dayTime && currentTime.Hour < _nightTime) ? true:false;
        else
            return (currentTime.Hour >= _nightTime && currentTime.Hour < _dayTime) ? false: true;
    }
public:
    Alarm(DataConfig* dataConfig) : _dataConfig(dataConfig),_freDay(_dataConfig->_freDay),_freNight(_dataConfig->_freNight),_dayTime(_dataConfig->_dayTime),_nightTime(_dataConfig->_nightTime),_isRinging(false),totalSecond(0) {
        this->_timeSecAlarm =TOTAL_TIME_PER_DAY;
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
    void changeAlarm() {
        Rtc* rtc = Rtc::getInstance();
        uint8_t fre = 0;
        if(isDay()){
            fre = this->_freDay;
            this->_timeSecAlarm = (_freDay-1)*3600 + _minuteStart * 60 - rtc->getMinute()*60 + rtc->getSecondsPoint() - rtc->getSecond() ;
        }
        else {
            fre = this->_freNight;
            this->_timeSecAlarm = (_freNight-1)*3600 + _minuteStart * 60 + rtc->getSecondsPoint() - rtc->getSecond() - rtc->getMinute()*60 ;
        }   
        if(_minuteStart <= rtc->getMinute()) this->_timeSecAlarm += 3600;
        if(this->_timeSecAlarm > TOTAL_TIME_PER_DAY ) this->_timeSecAlarm = this->_timeSecAlarm - TOTAL_TIME_PER_DAY;
        Serial.println("Reset Alarm, next Time location: "+ String(this->_timeSecAlarm) + ",current sec: "+ Rtc::getInstance()->getSecondsPoint());
    }
    void hasChanged() override{
        if((_dataConfig->_freDay != this->_freDay) || (this->_freNight != _dataConfig->_freNight) ||
           (this->_dayTime != _dataConfig->_dayTime) || (this->_nightTime != _dataConfig->_nightTime) || 
           (this->_minuteStart != _dataConfig->_minuteStart)){    
                Serial.println("Alarm->hasSignalChange() has signal from config");
                this->_freDay = _dataConfig->_freDay;
                this->_freNight = _dataConfig->_freNight;
                this->_dayTime = _dataConfig->_dayTime;
                this->_nightTime = _dataConfig->_nightTime;
                this->_minuteStart = _dataConfig->_minuteStart;
                changeAlarm();
        }
    }
    
    void resetAlarm(){
        changeAlarm();  
    }
    
    int getRemainSecond(){
        Rtc* rtc = Rtc::getInstance();
        int compareValue = this->_timeSecAlarm - rtc->getSecondsPoint();
        // Serial.println("alarm: compareValue: "+ String(compareValue));
        if(compareValue <  0){
            this->_secondLeft = TOTAL_TIME_PER_DAY + compareValue;
            Serial.println("alarm: next day, use other fomular to caculate secondLeft: "+String(this->_secondLeft));
        }
        else {
            this->_secondLeft = compareValue;
        }
        // Serial.println("remain second: " +String(this->_secondLeft));
        return this->_secondLeft;
    }
    int getRemainMinute() {
        return (getRemainSecond()/60) +1;
    }
    void updateRemainTime(){
        Lcd* lcd = Lcd::getInstance();
        int minuteLeft = getRemainMinute();
        if(this->_isRinging){
            lcd->show(String(0),Lcd::TYPE_REMAIN_TIME_ALARM,3);
            return;
        }
        if(minuteLeft != this->_minuteLeft){
            this->_minuteLeft  = minuteLeft;
            Serial.println("Alarm->update remain time() minute left: " + String(this->_minuteLeft));
            if(minuteLeft > 99){
                lcd->show("99",Lcd::TYPE_REMAIN_TIME_ALARM,3);    
            }
            else{
                lcd->show(String(minuteLeft),Lcd::TYPE_REMAIN_TIME_ALARM,3);
            }
        
        }
    }
    bool getIsRinging() const {
        return this->_isRinging;
    }

    void checked(){
        this->_isRinging = false;
    }

    void runAlarm() {
        static uint8_t lastSec=0;
        static uint8_t lastRemainTime =0;
        static int coutdownShowLcd = 10;

        /** Alarm running*/  
        if(_isRinging){
            return;
        }
        Rtc* rtc = Rtc::getInstance();
        Lcd* lcd = Lcd::getInstance();
        rtc->getTime(currentTime);
        
        if(currentTime.Hour == this->_nightTime){
            if(currentTime.Minute == 30 && currentTime.Second == 0){
                ESP.restart();
            }
        }

        int delta = currentTime.Second - lastSec;
        if(delta == 0) return;
        int remainSecound = getRemainSecond();
        if(this->_secondLeft == 0){
            _isRinging = true;
            resetAlarm();
        }
        updateRemainTime();
        lastSec = currentTime.Second;
    }
};
Alarm* Alarm::_alarm= nullptr;