#ifndef CONNECTION__H
#define CONNECTION__H
#include <Arduino.h>
#include <WiFi.h>
#include <math.h>
#include <WiFiMulti.h>
#include "../config/Config.h"
#include "../../board/rtc/Rtc.h"
#include "../../board/dcom/Dcom.h"
#include "../../board/io/Io.h"

class Connection
{
private:
     struct WifiElement{
          String _name;
          String _pass;
          uint8_t _id;
          WifiElement* operator = (const WifiElement& wifiElement){
               this->_name    = wifiElement._name;
               this->_pass    = wifiElement._pass;
               this->_id      = wifiElement._id;
               return this;
          }
     };
     WifiElement _listWifi[2];
     WifiElement _currentWifiConnected;
     void toggleWifiConnecting();
     bool _isConnected;
     unsigned long _timeoutConnection;
     uint8_t _timesMainWifiReconnect;
     static Connection* _connection;
private:
     WiFiMulti wifiMulti;
     int _isTimeout;
public:
     static Connection* getInstance();
     Connection();
     void init();
     bool isConnected() const;
     void threadjoin();
     void checkingConnection();
};

#endif //CONNECTION__H
