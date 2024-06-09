#ifndef CONNECTION__H
#define CONNECTION__H
#include <Arduino.h>
#include <WiFi.h>
#include <math.h>

#include "../../board/rtc/Rtc.h"
#include "../../board/dcom/Dcom.h"
#include "../../board/io/Io.h"
#define WIFI_ID_1     "Minh Tuan"
#define WIFI_PASS_1   "j12345678"
#define WIFI_ID_2     "Phuc Long"
#define WIFI_PASS_2   "Luckytea"
#define TIME_MAIN_WIFI_RECONNECT 2
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
public:
     static Connection* getInstance();
     Connection();
     void init();
     bool isConnected() const;
     void threadjoin();
     void checkingConnection();
};

#endif //CONNECTION__H
