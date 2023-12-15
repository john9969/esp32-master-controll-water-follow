#ifndef BOARD_UART_H
#define BOARD_UART_H
#include <Arduino.h>
#include <iostream>
#include <thread>
#include <chrono>
#include "lib/rtc/DS1307RTC.h"
typedef tmElements_t Time;
#define TOTAL_TIME_1_DAY 86400UL
class Rtc : public DS1307RTC
{

private:
    Rtc(){

    }
    struct Alarm{
        bool enable;
        int timeRemain;
        bool isRingging;
        bool hasChecked;
        uint32_t nextTime;
        int fre;
    };
    static Alarm alarm;
    static Rtc* rtc;
    static Time current;
    
    // static 
public:
    static bool setAlarm(const Time& t, const int& frequencyPerHour = 1){
        if(rtc == nullptr){
            rtc = new Rtc();
        }
        alarm.enable =true;
        if(t.Minute > current.Minute){
            alarm.timeRemain = t.Minute - current.Minute + 60* (frequencyPerHour-1);
        }
        else {
            //alarm.timeRemain = t.Minute - current.Minute + 60+ 60* (frequencyPerHour-1);
            alarm.timeRemain = t.Minute - current.Minute + 60* frequencyPerHour ; 
        }
        current;

    }
    static bool getTime(Time& t){
        if(rtc == nullptr){
            rtc = new Rtc();
        }
        t = current;  
    }
    static bool setTime(const Time& t){
        
    }
    static void run(){
        DS1307RTC::read(current);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
};
Rtc* Rtc::rtc = nullptr;
Rtc::Alarm Rtc::alarm = Rtc::Alarm{0,0,0,0,0,0};
#endif