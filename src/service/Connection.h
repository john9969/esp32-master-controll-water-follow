#ifndef SERVICE_WIFI_H
#define SERVICE_WIFI_H
#include <Arduino.h>
#include <WiFi.h>
#include "board/rtc.h"
#include <math.h>
#include <thread>
#include <chrono>
#include "board/dcom.h"
#define WIFI_ID     "Minh Tuan"
#define WIFI_PASS   "j12345678"
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
          threadConnection = std::thread(&Connection::checkingConnection,this);
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
    _isConnected = false;
    
}
void Connection::checkingConnection(){
     Time time;
     while (1) 
     {     
          Rtc* rtc = Rtc::getRtc();
          rtc->getTime(time);
          if(time.Hour == 3 && time.Minute == 0&& time.Second == 0){
               WiFi.disconnect();
               std::this_thread::sleep_for(std::chrono::seconds(1));
               this->timeoutConnection = millis();
               std::thread threadReconnect(&dcomReset);
               threadReconnect.join();
               init();
               Serial.println("disconnect wifi");
          }
          if(WiFi.status() ==  WL_CONNECTED){
               this->timeoutConnection = millis();
               if(!_isConnected){
                    _isConnected = true;
                    setErrCode(ERR_WIFI_LOST_CONNECT);
               }
                    
          }
          else{

               if(_isConnected) _isConnected = false;
               Serial.println("connecting.. " + String(millis() - this->timeoutConnection));

          }
          if((millis() -this->timeoutConnection) > 30000){
               setErrCode(ErrCode::ERR_WIFI_LOST_CONNECT);
               std::thread threadReconnect(&dcomReset);
               threadReconnect.join();
               this->timeoutConnection = millis();
          }
          std::this_thread::sleep_for(std::chrono::seconds(1)); 
     }   
}




#endif