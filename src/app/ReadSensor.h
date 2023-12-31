#include <iostream>
#include <vector>
#include "board/io.h"
#include "board/uart.h"
#define TIMEOUT_SENSOR 60000
#define TIME_CHECKING_SENSOR 700
class ReadSensor{
public:
    struct DataSensor{
        uint16_t round;
        uint16_t angle;
    };
    void init(){
        uartSensor.begin();
    }
    uint16_t readRound(){
        if(this->dataSensor.round <=0) return 0;
        uint16_t _round = this->dataSensor.round;
        this->dataSensor.round=0;
        return _round;
    }
    uint16_t readAngle(){
        return this->dataSensor.angle;
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
        String data = "";
        if((millis() - timeoutSensor) > TIMEOUT_SENSOR){
            setErrCode(ErrCode::ERR_SENSOR_FAIL);
        }
        if(isLed4WireOn){
            if((millis() - timeOnSignalLed) > 700){
                isLed4WireOn = false;
                setLow(LED_SIGNAL);
                // Serial.println("led low ");
            }
        }
        if(!uartSensor.read(data)){
            return;
        }
        if(!data.endsWith("(")){
            if(data.startsWith("start")){
                hasStartBtn = true;
            }
            return;
        }
        data.replace("(","");
        if(data != "*"){
            dataSensor.angle = data.toInt();
            dataSensor.round++;
            Serial.println("raw round: " + String(dataSensor.round) + "raw angle: "+ String(dataSensor.angle));
            isLed4WireOn = true;
            timeOnSignalLed = millis();
            setHigh(LED_SIGNAL);
        }
        else{
            removeErrCode(ErrCode::ERR_SENSOR_FAIL);
        }

        timeoutSensor = millis();
    
    }

    void process3Wire(){
        callback3wire();

        if((millis() - timeout3wireSignal) < 500) return;
        timeout3wireSignal = millis();
        if(isLedOn){
            setLow(LED_SIGNAL);
            isLedOn = false;
        }
        if(signalSensor3Wire){
            this->dataSensor.round++;
            isLedOn = true;
            Serial.println("round: "+ String(dataSensor.round));
            setHigh(LED_SIGNAL);
            signalSensor3Wire = false;
        }
    }
private:
    bool isLedOn;
    bool isLed4WireOn;
    uint64_t timeout3wireSignal;
    uint64_t timeOnSignalLed;
    bool hasSignalSensor;
    bool enableLed;
    static ReadSensor* readSensor;
    uint64_t timeoutSensor;
    DataSensor dataSensor;
    ReadSensor(): hasSignalSensor(false), dataSensor({0,0}),enableLed(false),timeOnSignalLed(0),isLedOn(false),
    timeout3wireSignal(0),timeoutSensor(0),isLed4WireOn(false){
        
    }

};
ReadSensor* ReadSensor::readSensor = nullptr;
