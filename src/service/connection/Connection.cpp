#include "Connection.h"
Connection* Connection::_connection = nullptr;
Connection* Connection::getInstance(){
    if(!_connection){
        _connection = new Connection();
    }
    return _connection;
}
Connection::Connection() :_isConnected(false){
    _timeoutConnection = 0;
    _connection = this;
}
bool Connection::isConnected() const { 
    return this->_isConnected;
}
void Connection::init(){
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    WiFi.begin(WIFI_ID_1, WIFI_PASS_1);
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
    _isConnected = false;
}
void Connection::checkingConnection(){
    Time time;
    Rtc* rtc = Rtc::getInstance();
    rtc->getTime(time);
    if(WiFi.status() ==  WL_CONNECTED){
        this->_timeoutConnection = millis();
        if(!isConnected()){
            _isConnected = true;
            removeErrCode(ErrCode::ERR_WIFI_LOST_CONNECT);
            Serial.println("wifi connected");
        }
    }
    else{
        if(_isConnected) _isConnected = false;
        Serial.print("connecting.. " );
        Serial.print("id:" + WiFi.SSID(1));
        Serial.println(String(millis() - this->_timeoutConnection));
    }
    if((millis() -this->_timeoutConnection) > 60000){
        if(dcomState == Dcom_State_ON){
            dcomState = Dcom_State_INIT_RESET;
            Serial.println("Start Reset");
        }
        if(dcomState == Dcom_State_DONE_RESET){
            this->_timeoutConnection = millis();
            Serial.println("done Reset");
            init();
            dcomState = Dcom_State_ON;
        }
        dcomReset();
    }
}
