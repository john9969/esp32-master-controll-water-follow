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
    void init();
    static LogicControl* getInstance();
    State getState() const;
    void setState(const State& state);
private:
    LogicControl();
    static LogicControl* _logicControl;

    State state;
};
#endif
LogicControl* LogicControl::_logicControl = nullptr;
void LogicControl::run(){
    Alarm * alarm = Alarm::getInstance();
   if(!alarm->getIsRinging()) return;
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
void LogicControl::init(){
    Alarm* alarm = Alarm::getInstance();
    alarm->setMinuteAlarm(55);
}
LogicControl::LogicControl(){
    this->state = STATE_RES;
}
void LogicControl::setState(const State& state){
    this->state = state;
}
LogicControl::State LogicControl::getState() const {
    return this->state;
}
LogicControl* LogicControl::getInstance(){
    if(!_logicControl){
        _logicControl = new LogicControl();
    }
    return _logicControl;
}