#ifndef CONNECTION__H
#define CONNECTION__H
#include <Arduino.h>
#include <WiFi.h>
#include <math.h>

#include "../../board/rtc/Rtc.h"
#include "../../board/dcom/Dcom.h"
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
     unsigned long _timeoutConnection;
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
