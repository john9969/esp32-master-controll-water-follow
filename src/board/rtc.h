#ifndef BOARD_RTC_H
#define BOARD_RTC_H
#include <Arduino.h>
#include <iostream>
#include <thread>
#include <chrono>
#include "lcd.h"
#include "lib/rtc/DS1307RTC.h"
#define YEAR_OFFSET 1970
typedef tmElements_t Time;
class Rtc : public DS1307RTC
{
private:
    Rtc():current(Time{0,01,10,1,1,1,52}){
        rtc = this;
        threadRtc = std::thread(Rtc::run,this);
    }
    static Rtc* rtc;
    static void* run(void* _arg){
        Rtc* rtc = (Rtc*) _arg;
        std::shared_ptr<Lcd> lcd = Lcd::getInstance();
        uint8_t lastSec = 0;
        uint8_t lastDay = 0;
        while(1){
            Time p_current;
            if(rtc->read(p_current)){
                if(lastSec != p_current.Second){
                    rtc->current = p_current;
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
                Serial.println("Set manual time");
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    }
    std::thread threadRtc;
    Time current;
public:
    
    static Rtc* getRtc(){
        if(!rtc){
            rtc = new Rtc();
        }
        return rtc;
    }
    void joinThread(){
        if(this->threadRtc.joinable()){
            this->threadRtc.join();
        }
    }
    static void getTime(Time& t){
        Rtc* rtc = Rtc::getRtc();
        t = rtc->current;  
    }
    static void setTime(Time t){
        Rtc* rtc = Rtc::getRtc();
        rtc->write(t); 
    }
   
};
Rtc* Rtc::rtc = nullptr;
#endif