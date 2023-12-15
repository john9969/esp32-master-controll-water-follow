#include "board/rtc.h"
#include <iostream>
#include <chrono>
#include <thread>
class Alarm {
private:
    Time alarmTime;
    int frequencyPerHour;
    bool alarmSet;

    Alarm() : frequencyPerHour(1), alarmSet(false) {}

public:
    static Alarm& getInstance() {
        static Alarm instance; // Guaranteed to be destroyed and instantiated on first use.
        return instance;
    }

    // Set the alarm time
    void setTime(const Time& time) {
        alarmTime = time;
        alarmSet = true;
    }
    void getTime(Time& time) const {
        time = alarmTime;
    }

    // Set the alarm with a specified frequency (default is 1 per hour)
    void setAlarm(const Time& time, int frequencyPerHour = 1) {
        setTime(time);
        this->frequencyPerHour = frequencyPerHour;
    }

    // Start the alarm, ringing per hour based on the set time and frequency
    void startAlarm() {
        while (alarmSet) {
            std::this_thread::sleep_for(std::chrono::hours(1 / frequencyPerHour));
            ringAlarm();
        }
    }

private:
    // Private method to simulate the alarm ringing
    void ringAlarm() const {
        Time currentTime;
        getTime(currentTime);

        std::cout << "Alarm ringing at " << currentTime.hours << ":"
                  << currentTime.minutes << ":" << currentTime.seconds << std::endl;
    }
};