#include <iostream>
#include <vector>
#include "board/io.h"
#include "board/uart.h"
#define TIMEOUT_SENSOR 60000
#define TIME_CHECKING_SENSOR 700
class ReadSensor{
public:
    struct DataSensor{
        uint8_t round;
        uint8_t angle;
    };
    void init(){
        uartSensor.begin();
    }
    uint8_t readRound(){
        uint8_t _round = this->dataSensor.round;
        this->dataSensor.round=0;
        return _round;
    }
    void clearRound(){
        this->dataSensor.round =0;
    }
    static ReadSensor* getInstance(){
        if(!readSensor){
            readSensor = new ReadSensor();
        }
        return readSensor;
    }
    void process4wire(){
        uint64_t timeOnSignalLed=0;
        bool isLedOn = false;
        String data = "";
        if((millis() - timeoutSensor) > TIMEOUT_SENSOR){
            setErrCode(ErrCode::ERR_SENSOR_FAIL);
        }
        if(isLedOn){
            if((millis() - timeOnSignalLed) > 500){
                isLedOn = false;
                setLow(LED_SIGNAL);
            }
        }
        if(!uartSensor.read(data)){
        }
        if(data != "*"){
            dataSensor.angle = data.toInt();
            dataSensor.round++;
            isLedOn = true;
            timeOnSignalLed = millis();
            setHigh(LED_SIGNAL);
        }
        timeoutSensor = millis();
    
    }
private:
    bool hasSignalSensor;
    bool enableLed;
    static ReadSensor* readSensor;
    uint64_t timeoutSensor;
    DataSensor dataSensor;
    void process3Wire(){
        static bool isLedOn = false;

        if(isLedOn){
            setLow(LED_SIGNAL);
        }
        if(signalSensor3Wire){
            this->dataSensor.round++;
            isLedOn = true;
            Serial.println("round:"+ String(dataSensor.round));
            setHigh(LED_SIGNAL);
            signalSensor3Wire = false;
        }
    }
    
    void processBlinkLed(){
        while (1)
        {
        setHigh(LED_SIGNAL);
        setLow(LED_SIGNAL);
        this->enableLed = false;
        }
    }
    ReadSensor(): hasSignalSensor(false), dataSensor({0,0}),enableLed(false){
        
    }

};
ReadSensor* ReadSensor::readSensor = nullptr;
