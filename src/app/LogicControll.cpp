#include "LogicControll.h"
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
