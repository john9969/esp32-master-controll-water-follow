#include "service/Alarm.h"
#include "service/HttpRequest.h"
#include "board/uart.h"
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
    LogicControl();
    State getState() const;
    void setState(const State& state);
private:
    State state;
   Alarm* alarm;
};
