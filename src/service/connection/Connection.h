#ifndef CONNECTION__H
#define CONNECTION__H
#include <Arduino.h>
#include <WiFi.h>
#include <math.h>
#include <WiFiMulti.h>

#include "../../board/rtc/Rtc.h"
#include "../../board/dcom/Dcom.h"
#include "../../board/io/Io.h"
#define WIFI_ID_1     "Thuyen2"
#define WIFI_PASS_1   "0363862128"
#define WIFI_ID_2     "Minh Tuan"
#define WIFI_PASS_2   "j12345678"
#define TIME_MAIN_WIFI_RECONNECT 2
#define TIME_OUT_MULTI_WIFI  12
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
