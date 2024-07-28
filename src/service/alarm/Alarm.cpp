#include "Alarm.h"
#include "../../app/logicControl/LoginControl.h"
bool Alarm::isDay(){
    if(_nightTime > _dayTime)
        return (_currentTime.Hour >= _dayTime && _currentTime.Hour < _nightTime) ? true:false;
    else
        return (_currentTime.Hour >= _nightTime && _currentTime.Hour < _dayTime) ? false: true;
}
Alarm::Alarm(DataConfig* dataConfig) : _dataConfig(dataConfig),_freDay(_dataConfig->getFreDay()),_freNight(_dataConfig->getFreNight()),_dayTime(_dataConfig->getDayTime()),_nightTime(_dataConfig->getNightTime()),_isRinging(false),_totalSecond(0) {
    this->_timeSecAlarm = 99*60 + 23*3600;
    _secondLeft = 3600;
    _minuteLeft = 99;   
    _minuteStart = 55;
    _dataConfig->append(this);
}

Alarm* Alarm::getInstance() {
    if(!_alarm){
        _alarm = new Alarm(DataConfig::getInstance());
    }
    return _alarm;
}
void Alarm::changeAlarm() {
    Rtc* rtc = Rtc::getInstance();
    uint8_t fre = 0;
    if(isDay()){
        fre = this->_freDay;
    }
    else {
        fre = this->_freNight;
    }   

    if(_minuteStart > rtc->getMinute()){
        this->_timeSecAlarm = (fre - 1)*3600 + _minuteStart*60 - rtc->getMinute()*60 -rtc->getSecond()  + rtc->getSecondsPoint();  
    }
    else {
        this->_timeSecAlarm = fre*3600 + _minuteStart*60 - rtc->getMinute()*60 - rtc->getSecond() + rtc->getSecondsPoint();  
    }
    Serial.printf(  "Reset Alarm, next Time location:%d,"
                    "current sec point: %d, remain sec: %d", 
                    this->_timeSecAlarm, rtc->getSecondsPoint(),
                    this->_timeSecAlarm- rtc->getSecondsPoint()
                );
}
void Alarm::hasChanged() {
    if((_dataConfig->getFreDay() != this->_freDay) || (_dataConfig->getFreNight() != this->_freNight) ||
        (_dataConfig->getDayTime() != this->_dayTime) || (_dataConfig->getNightTime() != this->_nightTime) || 
        (_dataConfig->getMinuteStart() != this->_minuteStart)){    
            Serial.println("Alarm->hasSignalChange() has signal from config");
            this->_freDay = _dataConfig->getFreDay();
            this->_freNight = _dataConfig->getFreNight();
            this->_dayTime = _dataConfig->getDayTime();
            this->_nightTime = _dataConfig->getNightTime();
            this->_minuteStart = _dataConfig->getMinuteStart();
            changeAlarm();
    }
}

void Alarm::resetAlarm(){
    changeAlarm();  
}

int Alarm::getRemainSecond(){
    Rtc* rtc = Rtc::getInstance();
    this->_secondLeft = this->_timeSecAlarm - rtc->getSecondsPoint();
    return this->_secondLeft;
}
int Alarm::getRemainMinute() {
    return (getRemainSecond()/60) +1;
}
void Alarm::updateRemainTime(){
    Lcd* lcd = Lcd::getInstance();
    int minuteLeft = getRemainMinute();
    if(minuteLeft != this->_minuteLeft){
        this->_minuteLeft  = minuteLeft;
        Serial.println("Alarm->update remain time() minute left: " + String(this->_minuteLeft));
        if(this->_isRinging){
            lcd->show(String(0),Lcd::TYPE_REMAIN_TIME_ALARM,3);
        }
        else if(minuteLeft > 999){
            lcd->show("999",Lcd::TYPE_REMAIN_TIME_ALARM,3);    
        }
        else{
            lcd->show(String(minuteLeft),Lcd::TYPE_REMAIN_TIME_ALARM,3);
        }
    }
}
bool Alarm::getIsRinging() const {
    return this->_isRinging;
}

void Alarm::checked(){
    if(this->_isRinging) this->_isRinging = false;
}

void Alarm::runAlarm() {
    static uint8_t lastSec=0;
    static uint8_t lastRemainTime =0;
    static int coutdownShowLcd = 10;

    /** Alarm running*/  
    // if(_isRinging){
    //     return;
    // }
    Rtc* rtc = Rtc::getInstance();
    Lcd* lcd = Lcd::getInstance();
    rtc->getTime(_currentTime);
    
    if( (rtc->getHour()     == this->_nightTime)          &&
        (rtc->getMinute()   == TIME_RESET_MACHINE_POINT)  && 
        (rtc->getSecond()   == 00)                        &&
        (!_isRinging)){
            ESP.restart();
    }
    int delta = rtc->getSecond() - lastSec;
    if(delta == 0) return;
    int remainSecound = getRemainSecond();

    if(remainSecound <= 0){
        if(!_isRinging) {
            _isRinging = true;
            Serial.println("alarm ring");
        }
        resetAlarm();
    }
    updateRemainTime();
    lastSec = rtc->getSecond();
}
Alarm* Alarm::_alarm= nullptr;