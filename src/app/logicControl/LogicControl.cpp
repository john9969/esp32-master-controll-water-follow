#include "LoginControl.h"
#include "../../service/ota/otaController.h"
LogicControl* LogicControl::_logicControl = nullptr;
void LogicControl:: run()
{
    Alarm * alarm = Alarm::getInstance();
    OtaController * ota = OtaController::getInstance();
    if((ota->getState() != OtaController::STATE_CHECK_AVAILABLE) &&
       (ota->getState() != OtaController::STATE_RES)){
        alarm->checked();
        if(hasStartBtn) hasStartBtn = false;
        return;
    }
    if((!alarm->getIsRinging()) && (!hasStartBtn)) return;
    if(getState() != STATE_RES && getState() != STATE_ERR){
        if(isTimeoutTotal()){
            Serial.println("total timeout: C:" + String(millis())+ "begin: "+ String(this->_timeoutTotal));
            this->_httpRequest->post("total timeout run out at state: " + String(getState()), API_POST, HttpRequest::TYPE_CRITICAL);
            setErrMeasuring(getState());
            setState(STATE_ERR);
        }
    }
    
    switch (getState())
    {
        case STATE_RES:
            Serial.println("Start measuring");
            setState(STATE_START_GO_TO_BOTTOM);
            this->_timeoutTotal = millis();
            break;
        case STATE_START_GO_TO_BOTTOM:
        {
            static bool isFinish = false; //to distingwish begin and end of measuring process
            static int wait  = 200;
            if(isTimeoutStep(60000)){
                this->_httpRequest->post("step timeout tai luc check day ", API_POST, HttpRequest::TYPE_CRITICAL);
                setErrMeasuring(getState());
                setState(STATE_START_GO_TO_BOTTOM);
                break;
            }
            if(isSlaveFinishMoving()){
                isFinish =true;
            }
            if(!isFinish) break;
            if(wait >0) {
                wait--;
                break;
            }
            wait = 200;
            isFinish = false;
            Serial.println("finish check deep");
            this->_httpRequest->post("hoan thanh do do sau, di len diem day", API_POST, HttpRequest::TYPE_CRITICAL);
            setState(STATE_MOVING_TO_DAY);
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
                this->_httpRequest->post("step timeout at state: "+ String(getState()), API_POST, HttpRequest::TYPE_CRITICAL);
                setErrMeasuring(getState());
                setState(getState());
                break;
            }
            if(isSlaveFinishMoving()){
                this->_timeStartMeasuring = millis();
                this->_httpRequest->post("hoan thanh di chuyen toi state: "+ String(getState()), API_POST, HttpRequest::TYPE_CRITICAL);
                this->_isFinishRemoveFirst =false;
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
            if(!this->_isFinishRemoveFirst){
                if(this->_readSensor->readRound() > 0 || (getTimeInStep()> 10000)){
                    this->_isFinishRemoveFirst = true;
                    this->_timeBeginMeasuring = millis();
                    setOnSpeaker(1000);
                }
                else {
                    int countUp = 10 - this->getTimeInStep()/1000;

                    if((millis() - this->_oldSec) >= 1000){
                        this->_oldSec = millis();
                        this->_lcd->show(String(countUp), Lcd::TYPE_COUNT_UP,3);
                    }
                }
                break;
            }
            if(isTimeoutStep(150000)){
                setErrMeasuring(getState());
                setState(STATE_ERR);
            }
            int countUp = int((millis() - this->_timeBeginMeasuring)/1000);
            if((millis() - this->_oldSec) >= 1000){
                this->_oldSec = millis();
                this->_lcd->show(String(countUp), Lcd::TYPE_COUNT_UP,3);
            }
            
            
            if(countUp < this->_lowerThreadsold){
                uint16_t round = this->_readSensor->readRound();
                if(round <=0) break; 
                this->_dataMeasuring.dataSensor._round+= round;
                this->_lcd->show(String(this->_dataMeasuring.dataSensor._round/5),Lcd::TYPE_ROUND,3);
                this->_lcd->show(String(this->_readSensor->readAngle()), Lcd::TYPE_ANGLE,3);
                if((this->_dataMeasuring.dataSensor._round % 5) != 0){
                    setOnSpeaker(300);
                    break;
                }
                setOnSpeaker(700);
                if(this->_dataMeasuring.dataSensor._round == 5){
                    this->_dataMeasuring.dataSensor._angle = this->_readSensor->readAngle();
                }
                break;
            }
            else if(countUp <= this->_upperThreadsold){
                uint16_t round = this->_readSensor->readRound();
                if(round > 0){
                    this->_dataMeasuring.dataSensor._round+= round;
                    this->_lcd->show(String(this->_dataMeasuring.dataSensor._round/5),Lcd::TYPE_ROUND,3);
                    this->_lcd->show(String(this->_readSensor->readAngle()), Lcd::TYPE_ANGLE,3);
                    Serial.println("angle: " + String(this->_readSensor->readAngle()));
                    if((this->_dataMeasuring.dataSensor._round % 5) != 0) {
                        setOnSpeaker(300);
                        break;
                    }
                }
                if(this->_dataMeasuring.dataSensor._round == 0) break; 
                if((this->_dataMeasuring.dataSensor._round % 5) != 0) break;
                        
                if(this->_dataMeasuring.dataSensor._round == 5){
                    this->_dataMeasuring.dataSensor._angle = this->_readSensor->readAngle();
                }
                this->_dataMeasuring.time = countUp;
                this->_listDataMeasuring.push_back(this->_dataMeasuring);
                String data = "R:" + String(this->_dataMeasuring.dataSensor._round) 
                        + " T:" + String(this->_dataMeasuring.time)
                        + " A:" + String(this->_dataMeasuring.dataSensor._angle);  
                    this->_lcd->show(data,Lcd::TYPE_PRIVIOUS_DATA_MEASURING);
                this->_httpRequest->post("hoan thanh do toc dp (truoc nguong) at State: "+String(getState()) + " R: "+ String( this->_dataMeasuring.dataSensor._round) + " A: " + String( this->_dataMeasuring.dataSensor._angle)+ " T: " + String( this->_dataMeasuring.time), API_POST,HttpRequest::TYPE_CRITICAL);
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
                if(this->_dataMeasuring.dataSensor._round >= 5){
                    this->_dataMeasuring.dataSensor._round = int((this->_dataMeasuring.dataSensor._round + 2) / 5) * 5;
                    this->_dataMeasuring.time = this->_upperThreadsold;
                    String data = "R:" + String(this->_dataMeasuring.dataSensor._round) 
                        + " T:" + String(this->_dataMeasuring.time)
                        + " A:" + String(this->_dataMeasuring.dataSensor._angle);  
                    this->_lcd->show(String(this->_dataMeasuring.time),Lcd::TYPE_TIME,3);
                    this->_lcd->show(data,Lcd::TYPE_PRIVIOUS_DATA_MEASURING);
                    this->_httpRequest->post("finish measuring (toi nguong) at State: "+String(getState()) + " R: "+ String( this->_dataMeasuring.dataSensor._round) + " A: " + String( this->_dataMeasuring.dataSensor._angle)+ " T: " + String( this->_dataMeasuring.time), API_POST,HttpRequest::TYPE_CRITICAL);
                    this->_listDataMeasuring.push_back(this->_dataMeasuring);
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
                    this->_dataMeasuring.dataSensor._round = 0;
                    this->_dataMeasuring.dataSensor._angle = 0;
                    this->_dataMeasuring.time = this->_upperThreadsold;
                    this->_listDataMeasuring.push_back(this->_dataMeasuring);
                    this->_lcd->show(String(this->_dataMeasuring.time),Lcd::TYPE_TIME,3);
                    String data = "R:000 T:"+ String(this->_dataMeasuring.time)+" A:000";  
                    this->_lcd->show(data,Lcd::TYPE_PRIVIOUS_DATA_MEASURING);
                    this->_httpRequest->post("hoan thanh do toc do (toi nguong, vong =0) at State: "+String(getState()) + " R: "+ String( this->_dataMeasuring.dataSensor._round) + " A: " + String( this->_dataMeasuring.dataSensor._angle)+ " T: " + String( this->_dataMeasuring.time), API_POST,HttpRequest::TYPE_CRITICAL);
                    switch (getState())
                    {
                    case STATE_MEASURING_DAY:
                        this->_dataMeasuring.time = 90;
                        this->_listDataMeasuring.push_back(this->_dataMeasuring);
                        this->_isFastMeasuring = true;
                        setState(STATE_MOVING_TO_06);
                        break;
                    case STATE_MEASURING_08:
                        setState(STATE_MOVING_TO_06);
                        break;
                    case STATE_MEASURING_06:
                        if(this->_isFastMeasuring){
                            for(int i =0 ;i<3;i++){
                                this->_listDataMeasuring.push_back(_dataMeasuring); //04,2,m
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
    {
        uint64_t time = millis();
        uartSlave.send("A"+ String(STATE_MOVING_TO_RES));
        while (!isSlaveFinishMoving())
        {
            if(millis() - time > 25000){
                uartSlave.send("A"+ String(STATE_MOVING_TO_RES));
                time = millis();
            }
        }
        ESP.restart();        
        break;             
    }
    }
    
}
bool LogicControl::isTimeoutStep(const uint64_t& timeout){
    if((millis() - this->_timeoutStep) > timeout)
        return true;
    return false;
}
uint64_t LogicControl::getTimeInStep() const{
    return (millis() - this->_timeoutStep);
}
bool LogicControl::isTimeoutTotal(){
    if((millis() - this->_timeoutTotal) > 1200000)
        return true;
    return false;
}
void LogicControl::init(){
}

void LogicControl::setState(const State& state){
    Serial.println("switch state" + String (state));
    switch (state)
    {
    case STATE_MEASURING_DAY:
        this->_upperThreadsold = 120;
        this->_lowerThreadsold = 91;
    case STATE_MEASURING_08:
    case STATE_MEASURING_06:
    case STATE_MEASURING_04:
    case STATE_MEASURING_02:
    case STATE_MEASURING_MAT:
        if(state != STATE_MEASURING_DAY){
            this->_upperThreadsold = 90;
        this->_lowerThreadsold = 61;
        }
        setOnSpeaker(500);
        this->_timeStartMeasuring = millis();
        this->_isFinishRemoveFirst =false;
        this->_readSensor->clearRound();
        this->_dataMeasuring.dataSensor._round = 0;
        this->_dataMeasuring.time =0;
        this->_lcd->show("000",Lcd::TYPE_ROUND);
        this->_lcd->show("000",Lcd::TYPE_COUNT_UP);
        this->_lcd->show("000",Lcd::TYPE_ANGLE);
        break;
    case STATE_START_GO_TO_BOTTOM:
        this->_lcd->show("D.S",Lcd::TYPE_POS);
        break;
    case STATE_MOVING_TO_DAY:
        this->_lcd->show("DAY",Lcd::TYPE_POS);
        break;
    case STATE_MOVING_TO_08:
        this->_lcd->show("0.8",Lcd::TYPE_POS);
        break;
    case STATE_MOVING_TO_06:
        this->_lcd->show("0.6",Lcd::TYPE_POS);
        break;
    case STATE_MOVING_TO_04:
        this->_lcd->show("0.4",Lcd::TYPE_POS);
        break;        
    case STATE_MOVING_TO_02:
        this->_lcd->show("0.2",Lcd::TYPE_POS);
        break;
    case STATE_MOVING_TO_MAT:
        this->_lcd->show("MAT",Lcd::TYPE_POS);
        break;
    case STATE_MOVING_TO_RES:
        this->_lcd->show("RES",Lcd::TYPE_POS);
        break;
    case STATE_RES:
    {
        if(this->_alarm->getIsRinging()){
            this->_alarm->checked();
        }
        
        if(hasStartBtn){
            hasStartBtn = false;
        }
        if(this->_isFastMeasuring) this->_isFastMeasuring = false;
        std::vector<std::vector<String>> str_data;
        for(DataMeasuring dataMeasuring: this->_listDataMeasuring){
            std::vector<String> element;
            String str_round = String(dataMeasuring.dataSensor._round/5);
            padLeft(str_round,3);
            String str_angle = String(dataMeasuring.dataSensor._angle);
            padLeft(str_angle,3);
            String str_time = String(dataMeasuring.time);
            padLeft(str_time,3);
            element.push_back(str_round);
            element.push_back(str_time);
            element.push_back(str_angle);
            str_data.push_back(element);
        }
        this->_httpRequest->post(str_data, API_POST);
        this->_listDataMeasuring.clear();
        break;
    }
    }
    int numState = int(state);
    if(numState <= 8){
        uartSlave.send("A" + String(state));
        uartSlave.clearBuffer();
        setOnSpeaker(3000);
        this->_httpRequest->post("hoan thanh do toc do at State: "+String(getState()) + " R: "+ String( this->_dataMeasuring.dataSensor._round) + " A: " + String( this->_dataMeasuring.dataSensor._angle)+ " T: " + String( this->_dataMeasuring.time), API_POST,HttpRequest::TYPE_CRITICAL);
    }
    
    this->_state = state;
    this->_timeoutStep = millis();
}
LogicControl::State LogicControl::getState() const {
    return this->_state;
}
LogicControl* LogicControl::getInstance(){
    if(!_logicControl){
        _logicControl = new LogicControl();
    }
    return _logicControl;
}
LogicControl::LogicControl():               _errMeasuring(ErrCode::NO_ERR),             _dataMeasuring({{0,0},0}),
    _readSensor(ReadSensor::getInstance()), _httpRequest(HttpRequest::getInstance()),   _oldSec(0),_lcd(Lcd::getInstance()),
    _upperThreadsold(90),                   _lowerThreadsold(61),                       _alarm(Alarm::getInstance()),
    _timeoutStep(0),                        _state(STATE_RES)
{
    //__constructor
}
void LogicControl::setErrMeasuring(const LogicControl::State& state){
    switch (state)
    {
    case STATE_MEASURING_DAY:
        this->_errMeasuring = ErrCode::ERR_NON_RESPONSE_AT_STEP_TO_BOT;
        break;
    case STATE_MEASURING_08:
        this->_errMeasuring = ErrCode::ERR_NON_RESPONSE_AT_STEP_TO_08;
        break;
    case STATE_MEASURING_06:
    this->_errMeasuring = ErrCode::ERR_NON_RESPONSE_AT_STEP_TO_06;
        break;
    case STATE_MEASURING_04:
        this->_errMeasuring = ErrCode::ERR_NON_RESPONSE_AT_STEP_TO_04;
        break;
    case STATE_MEASURING_02:
        this->_errMeasuring = ErrCode::ERR_NON_RESPONSE_AT_STEP_TO_02;
        break;
    case STATE_MEASURING_MAT:
    this->_errMeasuring = ErrCode::ERR_NON_RESPONSE_AT_STEP_TO_BOT;
        break;
    case STATE_MOVING_TO_DAY:
    this->_errMeasuring = ErrCode::ERR_NON_RESPONSE_AT_STEP_TO_BOT;
        break;
    case STATE_MOVING_TO_08:
    this->_errMeasuring = ErrCode::ERR_NON_RESPONSE_AT_STEP_TO_BOT;
        break;
    case STATE_MOVING_TO_06:
    this->_errMeasuring = ErrCode::ERR_NON_RESPONSE_AT_STEP_TO_BOT;
        break;
    case STATE_MOVING_TO_04:
    this->_errMeasuring = ErrCode::ERR_NON_RESPONSE_AT_STEP_TO_BOT;
        break;
    case STATE_MOVING_TO_02:
        this->_errMeasuring = ErrCode::ERR_NON_RESPONSE_AT_STEP_TO_BOT;
        break;
    case STATE_MOVING_TO_MAT:
        this->_errMeasuring = ErrCode::ERR_NON_RESPONSE_AT_STEP_TO_BOT;
        break;
    case STATE_MOVING_TO_RES:
        this->_errMeasuring = ErrCode::ERR_NON_RESPONSE_AT_STEP_TO_BOT;
        break;
    default:
        break;
    }
    setErrCode(this->_errMeasuring);
}

bool LogicControl::isSlaveFinishMoving(){
    String data ="";
#if !ENABLE_READ_SLAVE
        return true;
#else
    if(uartSlave.read(data)){
        Serial.println("data from slave: " + data);
        return data.startsWith("*")? true : false;
    }
    return false;
#endif
}
void LogicControl::padLeft(String& data, const uint8_t& length){
    if(data.length() >= length) return;
    String append = "";
    for(uint8_t i =0; i < (length - data.length()); i++){
        append+="0";
    }
    data = append + data;
}