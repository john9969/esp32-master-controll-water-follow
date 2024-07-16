#include "Connection.h"
Connection* Connection::_connection = nullptr;
Connection* Connection::getInstance(){
    if(!_connection){
        _connection = new Connection();
    }
    return _connection;
}
Connection::Connection() :  _isConnected(false),_timeoutConnection(0), _timesMainWifiReconnect(TIME_MAIN_WIFI_RECONNECT),_isTimeout(0){
    this->_listWifi[0] = {WIFI_ID_1, WIFI_PASS_1,    0}; 
    this->_listWifi[1] = {WIFI_ID_2, WIFI_PASS_2,    1};
    this->_currentWifiConnected = this->_listWifi[0];
}
bool Connection::isConnected() const { 
    return this->_isConnected;
}
void Connection::init(){
    delay(10);
    WiFi.mode(WIFI_STA);
    wifiMulti.addAP(WIFI_ID_1, WIFI_PASS_1);
    wifiMulti.addAP(WIFI_ID_2, WIFI_PASS_2);
    _isConnected = false;
}
void Connection::checkingConnection(){
    // if(this->_currentWifiConnected._id ==0 && findErrCode(ErrCode::ERR_WIFI_1_SIGNAL_WEAK)){
    //     log("signal wifi 1: " + this->_currentWifiConnected._name + " is weak");
    //     this->_currentWifiConnected = this->_listWifi[1];
    //     removeErrCode(ErrCode::ERR_WIFI_1_SIGNAL_WEAK);
    //     this->_isConnected = false;
    //     init();
    //     return;
    // }
    // if(findErrCode(ErrCode::ERR_WIFI_2_SIGNAL_WEAK)){
    //     log("signal wifi 2: " + this->_currentWifiConnected._name + " is weak");
    //     this->_currentWifiConnected = this->_listWifi[0];
    //     removeErrCode(ErrCode::ERR_WIFI_2_SIGNAL_WEAK);
    //     this->_isConnected  =false;
    //     // init();
    //     return;
    // }
    if(wifiMulti.run() ==  WL_CONNECTED){
        this->_isTimeout =0;
        if(!isConnected()){
            _isConnected = true;
            removeErrCode(ErrCode::ERR_WIFI_1_LOST_CONNECT);
            // if(_currentWifiConnected._id == 0 && findErrCode(ErrCode::ERR_WIFI_1_LOST_CONNECT))
            //     removeErrCode(ErrCode::ERR_WIFI_1_LOST_CONNECT);
            // else if(_currentWifiConnected._id == 1 && findErrCode(ErrCode::ERR_WIFI_2_LOST_CONNECT))
            //     removeErrCode(ErrCode::ERR_WIFI_2_LOST_CONNECT);
            log("wifi_ip: " + String(WiFi.localIP()) + " connected");
        }
    }
    else {
        if(isConnected()) _isConnected = false;
        log("connecting... " + String(++this->_isTimeout));
        log(String(millis() - this->_timeoutConnection));
    }
    // if((millis() - this->_timeoutConnection) > 12){
    if(this->_isTimeout >= TIME_OUT_MULTI_WIFI){
        if(dcomState == Dcom_State_ON){
            dcomState = Dcom_State_INIT_RESET;
            Serial.println("Start Reset");
        }
        if(dcomState == Dcom_State_DONE_RESET){
            this->_timeoutConnection = millis();
            Serial.println("done Reset");
            setErrCode(ErrCode::ERR_WIFI_1_LOST_CONNECT);
            this->_isTimeout = 0;
            dcomState = Dcom_State_ON;
        }
        dcomReset();
    }
}
void Connection::toggleWifiConnecting(){
    int length = sizeof(this->_listWifi)/sizeof(this->_listWifi[0]);
    
    for( int i=0;i < length;i++){
        if(this->_currentWifiConnected._id != i) continue;
        if(this->_currentWifiConnected._id >= (length - 1)) {
            this->_currentWifiConnected = this->_listWifi[0];
            return;
        }
    }
}