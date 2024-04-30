#ifndef BOARD_RTC_H
#define BOARD_RTC_H
#include <Arduino.h>
#include <iostream>
#include "lcd.h"
#include "lib/rtc/DS1307RTC.h"
typedef tmElements_t Time;
class Rtc : public DS1307RTC
{
private:
    Rtc():
    DS1307RTC(), current(Time{0,01,10,1,1,1,22}){}
    static Rtc* rtc;
    uint8_t lastSec = 0;
    uint8_t lastDay = 0;
    uint64_t _totalSecDay =0;
    Time current;
public:
    void fromString(String data){

    }

    void run(){
        Time p_current;
        Lcd* lcd = Lcd::getInstance();
        if(read(p_current)){
            if(lastSec != p_current.Second){
                current = p_current;
                lastSec = p_current.Second;
                String str_time;
                if(p_current.Hour <10)str_time = "0"+ String(p_current.Hour)+":";
                else str_time = String(p_current.Hour)+":";
                if(p_current.Minute <10)str_time += "0"+ String(p_current.Minute)+":";
                else str_time += String(p_current.Minute)+":";
                if(p_current.Second <10)str_time += "0"+ String(p_current.Second);
                else str_time += String(p_current.Second);
                lcd->show( str_time,Lcd::TYPE_TIME);
                if(lastDay != p_current.Day){
                    lastDay = p_current.Day;
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
    uint64_t getSecondsPoint(){
        return (this->current.Hour*3600 + this->current.Minute*60 + this->current.Second);
    }
    static Rtc* getInstance(){
        if(!rtc){
            rtc = new Rtc();
        }
        return rtc;
    }
    void getTime(Time& t){
        t = rtc->current;  
    }
    uint8_t getSecond(){
        return rtc->current.Second;
    }
    uint8_t getMinute(){
        return rtc->current.Minute;
    }
    uint8_t getHour(){
        return rtc->current.Hour;
    }
    void setTime(Time t){
        rtc->write(t); 
    }
   
};
Rtc* Rtc::rtc = nullptr;

#endif