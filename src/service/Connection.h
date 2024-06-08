#ifndef SERVICE_WIFI_H
#define SERVICE_WIFI_H
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include "board/rtc.h"
#include <math.h>
#include <thread>
#include <chrono>
#include "board/dcom.h"
#define WIFI_ID_1     "Minh Tuan"
#define WIFI_PASS_1   "j12345678"
#define WIFI_ID_2     "Minh Tuan"
#define WIFI_PASS_2   "j12345678"

// #define WIFI_ID     "Cong Ca Phe"
// #define WIFI_PASS   "congcaphe.com"
class Connection
{
private:
     bool _isConnected;
     unsigned long timeoutConnection;
     std::thread threadConnection;
     std::thread threadRequest;
     static Connection* connection;
public:
     static Connection* getInstance(){
          if(!connection){
               connection = new Connection();
          }
          return connection;
     }
     Connection() :_isConnected(false){
          timeoutConnection = 0;
          this->threadConnection = std::thread(&Connection::checkingConnection,this);
          connection = this;
     }
     void init();
     bool isConnected() const { return this->_isConnected;}
     void threadjoin(){
          if(this->threadConnection.joinable()){
               this->threadConnection.join();
          }         
     }
     
     void checkingConnection();
};
Connection* Connection::connection = nullptr;

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
          this->timeoutConnection = millis();
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
          Serial.println(String(millis() - this->timeoutConnection));
     }
     if((millis() -this->timeoutConnection) > 60000){
          if(dcomState == Dcom_State_ON){
               dcomState = Dcom_State_INIT_RESET;
               Serial.println("Start Reset");
          }
          if(dcomState == Dcom_State_DONE_RESET){
               this->timeoutConnection = millis();
               Serial.println("done Reset");
               init();
               dcomState = Dcom_State_ON;
          }
          dcomReset();
     }
}

#endif