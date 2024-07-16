#include "Alarm.h"
bool Alarm::isDay(){
    if(_nightTime > _dayTime)
        return (_currentTime.Hour >= _dayTime && _currentTime.Hour < _nightTime) ? true:false;
    else
        return (_currentTime.Hour >= _nightTime && _currentTime.Hour < _dayTime) ? false: true;
}
Alarm::Alarm(DataConfig* dataConfig) : _dataConfig(dataConfig),_freDay(_dataConfig->getFreDay()),_freNight(_dataConfig->getFreNight()),_dayTime(_dataConfig->getDayTime()),_nightTime(_dataConfig->getNightTime()),_isRinging(false),_totalSecond(0) {
    this->_timeSecAlarm =TOTAL_TIME_PER_DAY;
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
    std::shared_ptr<HttpRequest> http = HttpRequest::getInstance();
    int compareValue = this->_timeSecAlarm - rtc->getSecondsPoint();
    if(compareValue <  -10){
        this->_secondLeft = TOTAL_TIME_PER_DAY + compareValue;
        Serial.println("alarm: next day, use other fomular to caculate secondLeft: "+String(this->_secondLeft));
    }
    else {
        this->_secondLeft = compareValue;
    }
    // Serial.println("remain second: " +String(this->_secondLeft));
    return this->_secondLeft;
}
int Alarm::getRemainMinute() {
    return (getRemainSecond()/60) +1;
}
void Alarm::updateRemainTime(){
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
bool Alarm::getIsRinging() const {
    return this->_isRinging;
}

void Alarm::checked(){
    this->_isRinging = false;
}

void Alarm::runAlarm() {
    static uint8_t lastSec=0;
    static uint8_t lastRemainTime =0;
    static int coutdownShowLcd = 10;

    /** Alarm running*/  
    if(_isRinging){
        return;
    }
    Rtc* rtc = Rtc::getInstance();
    Lcd* lcd = Lcd::getInstance();
    rtc->getTime(_currentTime);
    
    if(_currentTime.Hour == this->_nightTime){
        if((_currentTime.Hour == this->_nightTime) 
            && (_currentTime.Minute == TIME_RESET_MACHINE_POINT) 
            && (_currentTime.Second == 00)){
                ESP.restart();
        }
    }

    int delta = _currentTime.Second - lastSec;
    if(delta == 0) return;
    int remainSecound = getRemainSecond();
    if(this->_secondLeft == 0){
        _isRinging = true;
        resetAlarm();
    }
    updateRemainTime();
    lastSec = _currentTime.Second;
}
Alarm* Alarm::_alarm= nullptr;