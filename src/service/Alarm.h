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
    uint32_t totalSecond;
    std::thread threadAlarm;
    Alarm() : frequencyPerHour(1),isRinging(false),checked(false),totalSecond(0) {
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

    // Set the alarm time only set minute
    void setAlarm(const Time& time) {
        nextTime = time;
    }
    void setMinuteAlarm(const uint8_t & minute){
        Time setTime = {0,minute,0,0,0,0,0};
        setAlarm(setTime);
    }
    int getRemainTime() const {
        return this->remainTime;
    }
    bool getIsRinging() const {
        return this->isRinging;
    }
private:
    // Start the alarm, ringing per hour based on the set time and frequency
    void runAlarm(void* _arg) {
        Alarm* self = (Alarm*)_arg;
        Rtc* rtc = Rtc::getRtc();
        uint8_t lastSec=0;
        std::shared_ptr<Lcd> lcd = Lcd::getInstance();     
        int coutdownShowLcd = 10;  
        while (1) {
            if(self->isRinging){
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }
            Time current;
            rtc->getTime(current);
            int value = self->nextTime.Minute - current.Minute;
            if(lastSec == current.Second) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }
            lastSec = current.Second;
            if(value == 0){
                if(self->nextTime.Second == current.Second){
                    self->isRinging = true;
                    lcd->show(String(self->remainTime),Lcd::TYPE_REMAIN_TIME_ALARM,3);
                }   
            }
            else if(value > 0)  {
                self->remainTime = value;
            }
            else{
                self->remainTime = (value + 60);
            }
            lcd->show(String(self->remainTime),Lcd::TYPE_REMAIN_TIME_ALARM,3);
            Serial.println("remain time" + String(self->remainTime));
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
};
Alarm* Alarm::_alarm = nullptr;

