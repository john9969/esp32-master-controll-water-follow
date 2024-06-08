#ifndef READSENSOR__H
#define READSENSOR__H

#include <iostream>
#include <vector>
#include "../../board/io/Io.h"
#include "../../board/uart/Uart.h"
#define TIMEOUT_SENSOR 60000
#define TIME_CHECKING_SENSOR 700
class ReadSensor{
public:
    struct DataSensor{
        uint16_t _round;
        uint16_t _angle;
    };
    void init();
    uint16_t readRound();
    uint16_t readAngle();
    void clearRound();
    static ReadSensor* getInstance();
    void process4wire();
    void process3Wire();
private:
    ReadSensor();
    bool _isLedOn;
    bool _isLed4WireOn;
    uint64_t _timeout3wireSignal;
    uint64_t _timeOnSignalLed;
    bool _hasSignalSensor;
    bool _enableLed;
    static ReadSensor* _readSensor;
    uint64_t _timeoutSensor;
    DataSensor _dataSensor;
};
#endif