#ifndef APP_LOGIC_CONTROL_H
#define APP_LOGIC_CONTROL_H
#include "ReadSensor.h"
#include "service/Alarm.h"
#include "service/HttpRequest.h"
class LogicControl{
public:
    enum State{
        STATE_RES=0,
        STATE_MEASURING =1,
        STATE_DAY=2,
        STATE_08=3,
        STATE_06=4,
        STATE_04=5,
        STATE_02=6,
        STATE_MAT=7,
    };
    void run();
    static std::shared_ptr<LogicControl> getInstance();
    LogicControl();
    State getState() const;
    void setState(const State& state);
private:
    static std::shared_ptr<LogicControl> logicControl;
    State state;
    Alarm* alarm;
};
#endif
std::shared_ptr<LogicControl> LogicControl::logicControl = nullptr;
void LogicControl::run(){
while (1)
{
   if(!this->alarm->getIsRinging()) continue;
    switch (getState())
    {
        case STATE_MEASURING:
            
            break;
        case STATE_DAY:
            break;
        case STATE_08:
            break;
        case STATE_06:
            break;
        case STATE_04:
            break;
        case STATE_02:
            break;
        case STATE_MAT:
            break;
    }
}
}

LogicControl::LogicControl(){
    this->alarm = Alarm::getInstance();
    this->alarm->setMinuteAlarm(55);
    this->state = STATE_RES;
}
void LogicControl::setState(const State& state){
    this->state = state;
}
LogicControl::State LogicControl::getState() const {
    return this->state;
}
std::shared_ptr<LogicControl> LogicControl::getInstance(){
    if(!logicControl){
        logicControl = std::shared_ptr<LogicControl>(new LogicControl());
    }
    return logicControl;
}