#include "Rtc.h"
Rtc::Rtc(): DS1307RTC(), _current(Time{0,01,10,1,1,1,22}){

}
void Rtc::fromString(String data){

}
/**
 * @brief: get string Rtc
 * @data: DD/MM/YY HH/MM/SS
 * @return: -1: read false, 1: read true
 */
int8_t Rtc::toString(String& data){
    Time p_current;
    if(!read(p_current)) return -1;
    data += String(p_current.Year);
    return -1;
}
void Rtc::run(){
    Time p_current;
    Lcd* lcd = Lcd::getInstance();
    if(read(p_current)){
        if(_lastSec != p_current.Second){
            _current = p_current;
            _lastSec = p_current.Second;
            String str_time;
            if(p_current.Hour <10)str_time = "0"+ String(p_current.Hour)+":";
            else str_time = String(p_current.Hour)+":";
            if(p_current.Minute <10)str_time += "0"+ String(p_current.Minute)+":";
            else str_time += String(p_current.Minute)+":";
            if(p_current.Second <10)str_time += "0"+ String(p_current.Second);
            else str_time += String(p_current.Second);
            lcd->show( str_time,Lcd::TYPE_TIME);
            if(_lastDay != p_current.Day){
                _lastDay = p_current.Day;
                String str_date;
                if(p_current.Day <10)str_date = "0"+ String(p_current.Day)+":";
                else str_date = String(p_current.Day)+":";
                if(p_current.Month <10)str_date += "0"+ String(p_current.Month)+":";
                else str_date += String(p_current.Month)+":";
                if(p_current.Year <10)str_date += "0"+ String(p_current.Year);
                else str_date += String(p_current.Year);
                lcd->show( str_date,Lcd::TYPE_DATE);
            }
        }
        
    }
    else{
        setTime(Time{0,01,10,1,1,1,22});
    }
}
uint64_t Rtc::getSecondsPoint(){
    return (this->_current.Hour*3600 + this->_current.Minute*60 + this->_current.Second);
}
Rtc* Rtc::getInstance(){
    if(!_rtc){
        _rtc = new Rtc();
    }
    return _rtc;
}
void Rtc::getTime(Time& t){
    t = _rtc->_current;  
}
uint8_t Rtc::getSecond(){
    return _rtc->_current.Second;
}
uint8_t Rtc::getMinute(){
    return _rtc->_current.Minute;
}
uint8_t Rtc::getHour(){
    return _rtc->_current.Hour;
}
void Rtc::setTime(Time t){
    _rtc->write(t); 
}
Rtc* Rtc::_rtc = nullptr;