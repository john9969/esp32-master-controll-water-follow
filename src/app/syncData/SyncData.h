#ifndef SYNC_DATA__H
#define SYNC_DATA__H
#include "../logicControl/LoginControl.h"
#include "../../board/readAdc/readAdc.h"
#include "service/config/Config.h"
class SyncData{
public:
    void syncErr();
    void syncTime();
    void syncConfig();
private:
    int _coutdown_syncTime;
    int _coutdown_syncConfig;
    
};
#endif