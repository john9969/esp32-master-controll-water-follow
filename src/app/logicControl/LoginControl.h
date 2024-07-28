#ifndef LOGIC_CONTROL__H
#define LOGIC_CONTROL__H

#include "../readSensor/ReadSensor.h"
#include "../../service/alarm/Alarm.h"
#include "../../service/httpRequest/HttpRequest.h"

class LogicControl{
public:
    enum State{
        STATE_START_GO_TO_BOTTOM= 1,
        STATE_MOVING_TO_DAY     = 2,
        STATE_MOVING_TO_08      = 3,
        STATE_MOVING_TO_06      = 4,
        STATE_MOVING_TO_04      = 5,
        STATE_MOVING_TO_02      = 6,
        STATE_MOVING_TO_MAT     = 7,
        STATE_MOVING_TO_RES     = 8,
        
        STATE_MEASURING_DAY     = 9,
        STATE_MEASURING_08      = 10,
        STATE_MEASURING_06      = 11,
        STATE_MEASURING_04      = 12,
        STATE_MEASURING_02      = 13,
        STATE_MEASURING_MAT     = 14,
        STATE_RES               = 15,   
        STATE_ERR               = 16

    };
    struct DataMeasuring{
        ReadSensor::DataSensor dataSensor;
        uint16_t time;
    };
    
    void run();
    void init();
    static LogicControl* getInstance();
    State getState() const;
    void setState(const State& state);
private:
    LogicControl();
    void padLeft(String& data, const uint8_t& length);
    Lcd* _lcd;
    ReadSensor* _readSensor;
    Alarm* _alarm;
    bool isSlaveFinishMoving();
    void setErrMeasuring(const State& _err);
    bool isTimeoutStep(const uint64_t& timeout);
    uint64_t getTimeInStep() const;
    std::shared_ptr<HttpRequest> _httpRequest;
    bool isTimeoutTotal();
    bool _isFinishRemoveFirst;
    uint64_t _timeStartMeasuring;
    DataMeasuring _dataMeasuring;
    std::vector<DataMeasuring> _listDataMeasuring;
    ErrCode _errMeasuring;
    uint64_t _timeoutTotal;
    uint64_t _timeoutStep;
    uint64_t _timeBeginMeasuring;
    uint64_t _oldSec;
    uint16_t _oldRound;
    uint8_t _upperThreadsold;
    uint8_t _lowerThreadsold;
    static LogicControl* _logicControl;
    State _state;
    bool _isFastMeasuring = false;
};
#endif