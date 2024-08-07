#include "ReadSensor.h"

void ReadSensor::init(){
    uartSensor.begin();
}
uint16_t ReadSensor::readRound(){
    if(this->_dataSensor._round <=0) return 0;
    uint16_t _round = this->_dataSensor._round;
    this->_dataSensor._round=0;
    return _round;
}
uint16_t ReadSensor::readAngle(){
    return this->_dataSensor._angle;
}
void ReadSensor::clearRound(){
    this->_dataSensor._round =0;
}
ReadSensor* ReadSensor::getInstance(){
    if(!_readSensor){
        _readSensor = new ReadSensor();
    }
    return _readSensor;
}
void ReadSensor::process4wire(){
    String data = "";
    if((millis() - this->_timeoutSensor) > TIMEOUT_SENSOR){
        setErrCode(ErrCode::ERR_SENSOR_FAIL);
    }
    if(this->_isLed4WireOn){
        if((millis() - this->_timeOnSignalLed) > 700){
            this->_isLed4WireOn = false;
            setLow(LED_SIGNAL);
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
        _dataSensor._angle = data.toInt();
        _dataSensor._round++;
        Serial.println("raw round: " + String(_dataSensor._round) + "raw angle: "+ String(_dataSensor._angle));
        this->_isLed4WireOn = true;
        _timeOnSignalLed = millis();
        setHigh(LED_SIGNAL);
    }
    else{
        removeErrCode(ErrCode::ERR_SENSOR_FAIL);
    }

    this->_timeoutSensor = millis();

}

void ReadSensor::process3Wire(){
    callback3wire();
    if((millis() - this->_timeout3wireSignal) < 500) return;
    this->_timeout3wireSignal = millis();
    if(_isLedOn){
        setLow(LED_SIGNAL);
        _isLedOn = false;
    }
    if(signalSensor3Wire){
        this->_dataSensor._round = this->_dataSensor._round + 5;;
        _isLedOn = true;
        Serial.println("round: "+ String(_dataSensor._round));
        setHigh(LED_SIGNAL);
        signalSensor3Wire = false;
    }
}

ReadSensor::ReadSensor(): _hasSignalSensor(false),  _dataSensor({0,0}), _enableLed(false),
                          _timeOnSignalLed(0),      _isLedOn(false),    _timeout3wireSignal(0),
                          _timeoutSensor(0),        _isLed4WireOn(false){
    
}

ReadSensor* ReadSensor::_readSensor = nullptr;
