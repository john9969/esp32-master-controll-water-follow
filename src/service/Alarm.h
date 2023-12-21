#include <Arduino.h>
#include "board/rtc.h"
#include <iostream>
#include <chrono>
#include <thread>
class Alarm {
private:
    static Alarm* _alarm;
    int remainTime;
    Time nextTime;
    bool isRinging;
    bool checked;
    int frequencyPerHour;
    bool alarmSet;
    uint32_t totalSecond;
    std::thread threadAlarm;
    Alarm() : frequencyPerHour(1), alarmSet(false),isRinging(false),checked(false),totalSecond(0) {
        threadAlarm = std::thread(&Alarm::runAlarm,this, this);
        _alarm = this;
    }
    void ringAlarm() const {
        Time currentTime;
        std::cout << "Alarm ringing at " << nextTime.Hour << ":"
                  << nextTime.Minute << ":" << nextTime.Second << std::endl;
    }
public:
    void joinThread(){
        if(this->threadAlarm.joinable())
            this->threadAlarm.join();
    }
    static Alarm* getInstance() {
        if(!_alarm){
            _alarm = new Alarm();
        }
        return _alarm;
    }

    // Set the alarm time
    void setAlarm(const Time& time) {
        nextTime = time;
        alarmSet = true;
    }
    int getRemainTime() const {
        return this->remainTime;
    }
    // Start the alarm, ringing per hour based on the set time and frequency
    void runAlarm(void* _arg) {
        Alarm* self = (Alarm*)_arg;
        //Alarm* self = Alarm::getInstance();
        Rtc* rtc = Rtc::getRtc();
        
        while (1) {
            if(self->isRinging){
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }
            Time current;
            rtc->getTime(current);
            int value = self->nextTime.Minute - current.Minute;
            if(value == 0){
                if(self->nextTime.Second == current.Second){
                    self->isRinging = true;
                }   
            }
            else if(value > 0)  {
                self->remainTime = value;
            }
            else{
                self->remainTime = (value + 60);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
};
Alarm* Alarm::_alarm = nullptr;

