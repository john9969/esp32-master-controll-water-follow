#ifndef BOARD_RTC_H
#define BOARD_RTC_H
#include <Arduino.h>
#include <iostream>
#include <thread>
#include <chrono>
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
        while(1){
        Time p_current = rtc->current;
        rtc->read(p_current);
        Serial.println("Time:"+ String(p_current.Hour)+ ":" + String(p_current.Minute) + ":" + String(p_current.Second));
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