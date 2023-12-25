#ifndef SERVICE_WIFI_H
#define SERVICE_WIFI_H
#include <Arduino.h>
#include <WiFi.h>
#include "board/rtc.h"
#include <math.h>
#include <thread>
#include <chrono>
#include "board/dcom.h"
#define WIFI_ID     "Selex_Mtors_T2"
#define WIFI_PASS   "smartelectric"
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
    WiFi.begin(WIFI_ID, WIFI_PASS);
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
    _isConnected = false;
    
    
}
void Connection::checkingConnection(){
     Time time;
     Rtc* rtc = Rtc::getInstance();
     rtc->getTime(time);
     if(time.Hour == 3 && time.Minute == 0&& time.Second == 0){
          std::this_thread::sleep_for(std::chrono::seconds(1));
          this->timeoutConnection = millis();
          std::thread threadReconnect(&dcomReset);
          threadReconnect.join();
          Serial.println("disconnect wifi");
     }
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
     if((millis() -this->timeoutConnection) > 30000){
          if(dcomState == Dcom_State_ON){
               dcomState = Dcom_State_INIT_RESET;
               Serial.println("Start Reset");
          }
          if(dcomState == Dcom_State_DONE_RESET){
               this->timeoutConnection = millis();
               Serial.println("done Reset");
               dcomState = Dcom_State_ON;
          }
          dcomReset();
     }
}

#endif