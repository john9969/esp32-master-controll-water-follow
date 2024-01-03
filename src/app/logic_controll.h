#ifndef APP_LOGIC_CONTROL_H
#define APP_LOGIC_CONTROL_H
#include "ReadSensor.h"
#include "service/Alarm.h"
#include "service/HttpRequest.h"
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
    Lcd* lcd;
    ReadSensor* readSensor;
    Alarm* alarm;
    bool isSlaveFinishMoving();
    void setErrMeasuring(const State& _err);
    bool isTimeoutStep(const uint64_t& timeout);
    uint64_t getTimeInStep() const;
    std::shared_ptr<HttpRequest> httpRequest;
    bool isTimeoutTotal();
    bool isFinishRemoveFirst;
    uint64_t timeStartMeasuring;
    DataMeasuring dataMeasuring;
    std::vector<DataMeasuring> listDataMeasuring;
    ErrCode errMeasuring;
    uint64_t timeoutTotal;
    uint64_t timeoutStep;
    uint64_t timeBeginMeasuring;
    uint64_t oldSec;
    uint16_t oldRound;
    uint8_t upperThreadsold;
    uint8_t lowerThreadsold;
    static LogicControl* _logicControl;
    State state;
    bool isFastMeasuring = false;
};
#endif
LogicControl* LogicControl::_logicControl = nullptr;
void LogicControl:: run()
{
    Alarm * alarm = Alarm::getInstance();
    if((!alarm->getIsRinging()) && (!hasStartBtn)) return;
    if(getState() != STATE_RES && getState() != STATE_ERR){
            if(isTimeoutTotal()){
                Serial.println("total timeout: C:" + String(millis())+ "begin: "+ String(this->timeoutTotal));
                httpRequest->post("total timeout run out at state: " + String(getState()), API_POST, HttpRequest::TYPE_CRITICAL);
                setErrMeasuring(getState());
                setState(STATE_ERR);
            }
    }
    
    switch (getState())
    {
        case STATE_RES:
            Serial.println("Start measuring");
            setState(STATE_START_GO_TO_BOTTOM);
            this->timeoutTotal = millis();
            break;
        case STATE_START_GO_TO_BOTTOM:
        {
            if(isTimeoutStep(60000)){
                httpRequest->post("step timeout tai luc check day ", API_POST, HttpRequest::TYPE_CRITICAL);
                setErrMeasuring(getState());
                setState(STATE_START_GO_TO_BOTTOM);
                break;
            }
            if(isSlaveFinishMoving()){
                Serial.println("finish check deep");
                httpRequest->post("hoan thanh do do sau, di len diem day", API_POST, HttpRequest::TYPE_CRITICAL);
                setState(STATE_MOVING_TO_DAY);
                break;
            }
            break;
        }
        case STATE_MOVING_TO_DAY:
        case STATE_MOVING_TO_08:
        case STATE_MOVING_TO_06:
        case STATE_MOVING_TO_04:
        case STATE_MOVING_TO_02:
        case STATE_MOVING_TO_MAT:
        case STATE_MOVING_TO_RES:
            if(isTimeoutStep(60000)){
                httpRequest->post("step timeout at state: "+ String(getState()), API_POST, HttpRequest::TYPE_CRITICAL);
                setErrMeasuring(getState());
                setState(getState());
                break;
            }
            if(isSlaveFinishMoving()){
                this->timeStartMeasuring = millis();
                httpRequest->post("hoan thanh di chuyen toi state: "+ String(getState()), API_POST, HttpRequest::TYPE_CRITICAL);
                this->isFinishRemoveFirst =false;
                switch (getState())
                {
                    case STATE_MOVING_TO_DAY:
                        setState(STATE_MEASURING_DAY);
                        break;
                    case STATE_MOVING_TO_08:
                        setState(STATE_MEASURING_08);
                        break;
                    case STATE_MOVING_TO_06:
                        setState(STATE_MEASURING_06);
                        break;
                    case STATE_MOVING_TO_04:
                        setState(STATE_MEASURING_04);
                        break;
                    case STATE_MOVING_TO_02:
                        setState(STATE_MEASURING_02);
                        break;
                    case STATE_MOVING_TO_MAT:
                        setState(STATE_MEASURING_MAT);
                        break;
                    case STATE_MOVING_TO_RES:
                        setState(STATE_RES);
                        break;
                    default:
                        Serial.println("moving err at state:" +String(getState()));
                        break;
                }
                break;
            }
            break;
        case STATE_MEASURING_DAY:
        case STATE_MEASURING_08:
        case STATE_MEASURING_06:
        case STATE_MEASURING_04:
        case STATE_MEASURING_02:
        case STATE_MEASURING_MAT:
        {
            if(!isFinishRemoveFirst){
                if(readSensor->readRound() > 0 || (getTimeInStep()> 10000)){
                    isFinishRemoveFirst = true;
                    this->timeBeginMeasuring = millis();
                    setOnSpeaker(1000);
                }
                else {
                    int countUp = 10 - this->getTimeInStep()/1000;

                    if((millis() - this->oldSec) >= 1000){
                        this->oldSec = millis();
                        lcd->show(String(countUp), Lcd::TYPE_COUNT_UP,3);
                    }
                }
                break;
            }
            if(isTimeoutStep(150000)){
                setErrMeasuring(getState());
                setState(STATE_ERR);
            }
            int countUp = int((millis() - this->timeBeginMeasuring)/1000);
            if((millis() - this->oldSec) >= 1000){
                this->oldSec = millis();
                lcd->show(String(countUp), Lcd::TYPE_COUNT_UP,3);
            }
            
            
            if(countUp < lowerThreadsold){
                uint16_t _round = readSensor->readRound();
                if(_round <=0) break; 
                this->dataMeasuring.dataSensor.round+= _round;
                lcd->show(String(this->dataMeasuring.dataSensor.round),Lcd::TYPE_ROUND,3);
                lcd->show(String(readSensor->readAngle()), Lcd::TYPE_ANGLE,3);
                if((this->dataMeasuring.dataSensor.round % 5) != 0){
                    setOnSpeaker(300);
                    break;
                }
                setOnSpeaker(700);
                if(this->dataMeasuring.dataSensor.round == 5){
                    this->dataMeasuring.dataSensor.angle = readSensor->readAngle();
                }
                break;
            }
            else if(countUp <= upperThreadsold){
                uint16_t _round = readSensor->readRound();
                if(_round > 0){
                    this->dataMeasuring.dataSensor.round+= _round;
                    lcd->show(String(this->dataMeasuring.dataSensor.round),Lcd::TYPE_ROUND,3);
                    lcd->show(String(readSensor->readAngle()), Lcd::TYPE_ANGLE,3);
                    Serial.println("angle: " + String(readSensor->readAngle()));
                    if((this->dataMeasuring.dataSensor.round % 5) != 0) {
                        setOnSpeaker(300);
                        break;
                    }
                }
                if(this->dataMeasuring.dataSensor.round == 0) break; 
                if((this->dataMeasuring.dataSensor.round % 5) != 0) break;
                        
                if(this->dataMeasuring.dataSensor.round == 5){
                    this->dataMeasuring.dataSensor.angle = readSensor->readAngle();
                }
                this->dataMeasuring.time = countUp;
                this->listDataMeasuring.push_back(this->dataMeasuring);
                String data = "R:" + String(this->dataMeasuring.dataSensor.round) 
                        + " T:" + String(dataMeasuring.time)
                        + " A:" + String(this->dataMeasuring.dataSensor.angle);  
                    lcd->show(data,Lcd::TYPE_PRIVIOUS_DATA_MEASURING);
                this->httpRequest->post("hoan thanh do toc dp (truoc nguong) at State: "+String(getState()) + " R: "+ String( this->dataMeasuring.dataSensor.round) + " A: " + String( this->dataMeasuring.dataSensor.angle)+ " T: " + String( this->dataMeasuring.time), API_POST,HttpRequest::TYPE_CRITICAL);
                switch (getState())
                {
                case STATE_MEASURING_DAY:
                    setState(STATE_MOVING_TO_08);
                    break;
                case STATE_MEASURING_08:
                    setState(STATE_MOVING_TO_06);
                    break;
                case STATE_MEASURING_06:
                    setState(STATE_MOVING_TO_04);
                    break;
                case STATE_MEASURING_04:
                    setState(STATE_MOVING_TO_02);
                    break;
                case STATE_MEASURING_02:
                    setState(STATE_MOVING_TO_MAT);
                    break;
                case STATE_MEASURING_MAT:
                    setState(STATE_MOVING_TO_RES);
                    break;
                default:
                    break;
                }
                break;
            }
            else{ //round > upperthundershold
                if(this->dataMeasuring.dataSensor.round >= 5){
                    this->dataMeasuring.dataSensor.round = int((this->dataMeasuring.dataSensor.round + 2) / 5) * 5;
                    this->dataMeasuring.time = upperThreadsold;
                    String data = "R:" + String(this->dataMeasuring.dataSensor.round) 
                        + " T:" + String(dataMeasuring.time)
                        + " A:" + String(this->dataMeasuring.dataSensor.angle);  
                    lcd->show(String(this->dataMeasuring.time),Lcd::TYPE_TIME,3);
                    lcd->show(data,Lcd::TYPE_PRIVIOUS_DATA_MEASURING);
                    this->httpRequest->post("finish measuring (toi nguong) at State: "+String(getState()) + " R: "+ String( this->dataMeasuring.dataSensor.round) + " A: " + String( this->dataMeasuring.dataSensor.angle)+ " T: " + String( this->dataMeasuring.time), API_POST,HttpRequest::TYPE_CRITICAL);
                    this->listDataMeasuring.push_back(this->dataMeasuring);
                    switch (getState())
                    {
                    case STATE_MEASURING_DAY:
                        setState(STATE_MOVING_TO_08);
                        break;
                    case STATE_MEASURING_08:
                        setState(STATE_MOVING_TO_06);
                        break;
                    case STATE_MEASURING_06:
                        setState(STATE_MOVING_TO_04);
                        break;
                    case STATE_MEASURING_04:
                        setState(STATE_MOVING_TO_02);
                        break;
                    case STATE_MEASURING_02:
                        setState(STATE_MOVING_TO_MAT);
                        break;
                    case STATE_MEASURING_MAT:
                        setState(STATE_MOVING_TO_RES);
                        break;
                    default:
                        break;
                    }
                }
                else
                {
                    dataMeasuring.dataSensor.round = 0;
                    dataMeasuring.dataSensor.angle = 0;
                    dataMeasuring.time = upperThreadsold;
                    listDataMeasuring.push_back(dataMeasuring);
                    lcd->show(String(this->dataMeasuring.time),Lcd::TYPE_TIME,3);
                    String data = "R:000 T:"+ String(dataMeasuring.time)+" A:000";  
                    lcd->show(data,Lcd::TYPE_PRIVIOUS_DATA_MEASURING);
                    this->httpRequest->post("hoan thanh do toc do (toi nguong, vong =0) at State: "+String(getState()) + " R: "+ String( this->dataMeasuring.dataSensor.round) + " A: " + String( this->dataMeasuring.dataSensor.angle)+ " T: " + String( this->dataMeasuring.time), API_POST,HttpRequest::TYPE_CRITICAL);
                    switch (getState())
                    {
                    case STATE_MEASURING_DAY:
                        dataMeasuring.time = 90;
                        listDataMeasuring.push_back(dataMeasuring);
                        this->isFastMeasuring = true;
                        setState(STATE_MOVING_TO_06);
                        break;
                    case STATE_MEASURING_08:
                        setState(STATE_MOVING_TO_06);
                        break;
                    case STATE_MEASURING_06:
                        if(isFastMeasuring){
                            for(int i =0 ;i<3;i++){
                                this->listDataMeasuring.push_back(dataMeasuring); //04,2,m
                            }
                            setState(STATE_MOVING_TO_RES);
                            break;
                        }
                        setState(STATE_MOVING_TO_04);
                        break;
                    case STATE_MEASURING_04:
                        setState(STATE_MOVING_TO_02);
                        break;
                    case STATE_MEASURING_02:
                        setState(STATE_MOVING_TO_MAT);
                        break;
                    case STATE_MEASURING_MAT:
                        setState(STATE_MOVING_TO_RES);
                        break;
                    default:
                        break;
                    }
                    
                }
                
            }
            break;
        }   
    case STATE_ERR:
        
        break;             
    }
    
}
bool LogicControl::isTimeoutStep(const uint64_t& timeout){
    if((millis() - this->timeoutStep) > timeout)
        return true;
    return false;
}
uint64_t LogicControl::getTimeInStep() const{
    return (millis() - this->timeoutStep);
}
bool LogicControl::isTimeoutTotal(){
    if((millis() - this->timeoutTotal) > 1200000)
        return true;
    return false;
}
void LogicControl::init(){
    alarm->setMinuteAlarm(55);
}

void LogicControl::setState(const State& _state){
    Serial.println("switch state" + String (_state));
    switch (_state)
    {
    case STATE_MEASURING_DAY:
        this->upperThreadsold = 120;
        this->lowerThreadsold = 91;
    case STATE_MEASURING_08:
    case STATE_MEASURING_06:
    case STATE_MEASURING_04:
    case STATE_MEASURING_02:
    case STATE_MEASURING_MAT:
        if(_state != STATE_MEASURING_DAY){
            this->upperThreadsold = 90;
        this->lowerThreadsold = 61;
        }
        setOnSpeaker(500);
        this->timeStartMeasuring = millis();
        this->isFinishRemoveFirst =false;
        readSensor->clearRound();
        this->dataMeasuring.dataSensor.round = 0;
        this->dataMeasuring.time =0;
        lcd->show("000",Lcd::TYPE_ROUND);
        lcd->show("000",Lcd::TYPE_COUNT_UP);
        lcd->show("000",Lcd::TYPE_ANGLE);
        break;
    case STATE_START_GO_TO_BOTTOM:
        lcd->show("D.S",Lcd::TYPE_POS);
        break;
    case STATE_MOVING_TO_DAY:
        lcd->show("DAY",Lcd::TYPE_POS);
        break;
    case STATE_MOVING_TO_08:
        lcd->show("0.8",Lcd::TYPE_POS);
        break;
    case STATE_MOVING_TO_06:
        lcd->show("0.6",Lcd::TYPE_POS);
        break;
    case STATE_MOVING_TO_04:
        lcd->show("0.4",Lcd::TYPE_POS);
        break;        
    case STATE_MOVING_TO_02:
        lcd->show("0.2",Lcd::TYPE_POS);
        break;
    case STATE_MOVING_TO_MAT:
    lcd->show("MAT",Lcd::TYPE_POS);
        break;
    case STATE_MOVING_TO_RES:
    lcd->show("RES",Lcd::TYPE_POS);
        break;
    case STATE_RES:
    {
        if(alarm->getIsRinging()){
            alarm->resetAlarm();
        }
        if(hasStartBtn){
            hasStartBtn = false;
        }
        if(isFastMeasuring) isFastMeasuring = false;
        std::vector<std::vector<String>> str_data;
        // if(this->listDataMeasuring.size() > 7) setErrCode(ErrCode::ERR_NUM_OF_LIST_DATA_OUT_OF_RANGE);
        for(DataMeasuring dataMeasuring: this->listDataMeasuring){
            std::vector<String> element;
            String str_round = String(dataMeasuring.dataSensor.round);
            padLeft(str_round,3);
            String str_angle = String(dataMeasuring.dataSensor.angle);
            padLeft(str_angle,3);
            String str_time = String(dataMeasuring.time);
            padLeft(str_time,3);
            element.push_back(str_round);
            element.push_back(str_time);
            element.push_back(str_angle);
            str_data.push_back(element);
        }
        httpRequest->post(str_data, API_POST);
        this->listDataMeasuring.clear();
        break;
    }
    }
    int numState = int(_state);
    if(numState <= 8){
        uartSlave.send("A" + String(_state));
        uartSlave.clearBuffer();
        setOnSpeaker(3000);
        this->httpRequest->post("hoan thanh do toc do at State: "+String(getState()) + " R: "+ String( this->dataMeasuring.dataSensor.round) + " A: " + String( this->dataMeasuring.dataSensor.angle)+ " T: " + String( this->dataMeasuring.time), API_POST,HttpRequest::TYPE_CRITICAL);
    }
    
    this->state = _state;
    timeoutStep = millis();
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
LogicControl::LogicControl(): errMeasuring(ErrCode::NO_ERR), dataMeasuring({{0,0},0}),
    readSensor(ReadSensor::getInstance()),httpRequest(HttpRequest::getInstance()), oldSec(0),lcd(Lcd::getInstance()),
    upperThreadsold(90),lowerThreadsold(61), alarm(Alarm::getInstance()),timeoutStep(0)
{
    this->state = STATE_RES;
}
void LogicControl::setErrMeasuring(const LogicControl::State& state){
    switch (state)
    {
    case STATE_MEASURING_DAY:
        this->errMeasuring = ErrCode::ERR_NON_RESPONSE_AT_STEP_TO_BOT;
        break;
    
    case STATE_MEASURING_08:
    this->errMeasuring = ErrCode::ERR_NON_RESPONSE_AT_STEP_TO_08;
        break;
    
    case STATE_MEASURING_06:
    this->errMeasuring = ErrCode::ERR_NON_RESPONSE_AT_STEP_TO_06;
        break;
    
    case STATE_MEASURING_04:
    this->errMeasuring = ErrCode::ERR_NON_RESPONSE_AT_STEP_TO_04;
        break;
    case STATE_MEASURING_02:
        break;
    case STATE_MEASURING_MAT:
    this->errMeasuring = ErrCode::ERR_NON_RESPONSE_AT_STEP_TO_BOT;
        break;
    case STATE_MOVING_TO_DAY:
    this->errMeasuring = ErrCode::ERR_NON_RESPONSE_AT_STEP_TO_BOT;
    break;
    case STATE_MOVING_TO_08:
    this->errMeasuring = ErrCode::ERR_NON_RESPONSE_AT_STEP_TO_BOT;
    break;
    case STATE_MOVING_TO_06:
    this->errMeasuring = ErrCode::ERR_NON_RESPONSE_AT_STEP_TO_BOT;
    break;
    case STATE_MOVING_TO_04:
    this->errMeasuring = ErrCode::ERR_NON_RESPONSE_AT_STEP_TO_BOT;
    break;
    case STATE_MOVING_TO_02:
    this->errMeasuring = ErrCode::ERR_NON_RESPONSE_AT_STEP_TO_BOT;
    break;
    case STATE_MOVING_TO_MAT:
    this->errMeasuring = ErrCode::ERR_NON_RESPONSE_AT_STEP_TO_BOT;
    break;
    case STATE_MOVING_TO_RES:
    this->errMeasuring = ErrCode::ERR_NON_RESPONSE_AT_STEP_TO_BOT;
    break;
    default:
        break;
    }
    
    setErrCode(this->errMeasuring);
}
bool LogicControl::isSlaveFinishMoving(){
    String data ="";
    if(uartSlave.read(data)){
        Serial.println("data from slave: " + data);
        return data.startsWith("*")? true : false;
    }
    return false;
}
void LogicControl::padLeft(String& data, const uint8_t& length){
    if(data.length() >= length) return;
    String append = "";
    for(uint8_t i =0; i < (length - data.length()); i++){
        append+="0";
    }
    data = append + data;
}