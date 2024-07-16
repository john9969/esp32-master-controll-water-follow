#ifndef SYNC_DATA__H
#define SYNC_DATA__H
#include "../logicControl/LoginControl.h"
#include "../../board/readAdc/readAdc.h"
#include "../../service/config/Config.h"
#include "../../service/ota/otaController.h"
#define SYNC_OTA 5000
class SyncData{
public:
    void syncErr();
    void syncTime();
    void syncConfig();
    void syncBugOnline();
    void syncOta();
private:
    int _coutdown_syncTime;
    int _coutdown_syncConfig;
    
};
#endif