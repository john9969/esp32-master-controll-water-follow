#include "Connection.h"
Connection* Connection::_connection = nullptr;
Connection* Connection::getInstance(){
    if(!_connection){
        _connection = new Connection();
    }
    return _connection;
}
Connection::Connection() :  _isConnected(false),_timeoutConnection(0), _timesMainWifiReconnect(TIME_MAIN_WIFI_RECONNECT){
    this->_listWifi[0] = {WIFI_ID_1, WIFI_PASS_1,    0}; 
    this->_listWifi[1] = {WIFI_ID_2, WIFI_PASS_2,    1};
    this->_currentWifiConnected = this->_listWifi[0];
}
bool Connection::isConnected() const { 
    return this->_isConnected;
}
void Connection::init(){
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    WiFi.begin(this->_currentWifiConnected._name, this->_currentWifiConnected._pass);
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
    _isConnected = false;
}
void Connection::checkingConnection(){
    if(this->_currentWifiConnected._id ==0 && findErrCode(ErrCode::ERR_WIFI_1_SIGNAL_WEAK)){
        log("signal wifi 1: " + this->_currentWifiConnected._name + " is weak");
        this->_currentWifiConnected = this->_listWifi[1];
        removeErrCode(ErrCode::ERR_WIFI_1_SIGNAL_WEAK);
        this->_isConnected = false;
        init();
        return;
    }
    if(findErrCode(ErrCode::ERR_WIFI_2_SIGNAL_WEAK)){
        log("signal wifi 2: " + this->_currentWifiConnected._name + " is weak");
        this->_currentWifiConnected = this->_listWifi[0];
        removeErrCode(ErrCode::ERR_WIFI_2_SIGNAL_WEAK);
        this->_isConnected  =false;
        init();
        return;
    }
    if(WiFi.status() ==  WL_CONNECTED){
        this->_timeoutConnection = millis();
        if(this->_currentWifiConnected._id == 1){
            
        }
        if(!isConnected()){
            _isConnected = true;
            if(_currentWifiConnected._id == 0 && findErrCode(ErrCode::ERR_WIFI_1_LOST_CONNECT))
                removeErrCode(ErrCode::ERR_WIFI_1_LOST_CONNECT);
            else if(_currentWifiConnected._id == 1 && findErrCode(ErrCode::ERR_WIFI_2_LOST_CONNECT))
                removeErrCode(ErrCode::ERR_WIFI_2_LOST_CONNECT);
            log("wifi: "+ String(WiFi.getHostname()) +" connected");
        }
    }
    else {
        if(isConnected()) _isConnected = false;
        log("connecting... " );
        log(WiFi.SSID());
        log(String(millis() - this->_timeoutConnection));
    }
    if((millis() - this->_timeoutConnection) > 60000){
        if(dcomState == Dcom_State_ON){
            dcomState = Dcom_State_INIT_RESET;
            Serial.println("Start Reset");
        }
        if(dcomState == Dcom_State_DONE_RESET){
            this->_timeoutConnection = millis();
            Serial.println("done Reset");
            if((this->_currentWifiConnected._id == 0) && (--this->_timesMainWifiReconnect <= 0)){
                this->_currentWifiConnected = this->_listWifi[1];
            }
            init();
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