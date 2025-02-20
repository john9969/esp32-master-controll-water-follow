#include "SyncData.h"

void SyncData::syncErr(){
    Lcd* lcd = Lcd::getInstance();
    if(hasErrChanged){
        int numErr = err.size();
        if(numErr<= 0 ){
            lcd->show("NO_ER",Lcd::TYPE_ERR);
            return;
        }
        String str_err = "E:";
        for(ErrCode& _err : err){
            str_err += String(_err) +",";
        }
        std::shared_ptr<HttpRequest> http = HttpRequest::getInstance();
        http->post("new err: "  + String(str_err)," ", HttpRequest::TYPE_CRITICAL);
        lcd->show(str_err,Lcd::TYPE_ERR);
        hasErrChanged = false;
    }
}
void SyncData::syncConfig(){
    Rtc* rtc = Rtc::getInstance();
    Alarm * alarm = Alarm::getInstance();
    DataConfig* config = DataConfig::getInstance();
    OtaController* otaController = OtaController::getInstance();
    if(otaController->getState() != OtaController::STATE_CHECK_AVAILABLE &&
       otaController->getState() != OtaController::STATE_RES ) 
        return;
    if(alarm->getIsRinging() || hasStartBtn) return;
    if( _coutdown_syncConfig< 11) {
        _coutdown_syncConfig++;
        return;
    }
    std::shared_ptr<HttpRequest> httpRequest = HttpRequest::getInstance();
    String data = httpRequest->get(API_GET_CONFIG+ config->getSerialNumber());
    data.trim();
    if(data.length() <10) return;
    if(data.startsWith("config:")){
        data = data.substring(data.indexOf(":"),data.length());
        config->fromString(data);
        removeErrCode(ErrCode::ERR_GET_DATACONFIG_FAIL);
    }
    else {
        setErrCode(ErrCode::ERR_GET_DATACONFIG_FAIL);
    }
    _coutdown_syncConfig=0;
}
void SyncData::syncOta(){
#if ENABLE_OTA
    // Alarm * alarm = Alarm::getInstance();
    // if(alarm->getIsRinging() || hasStartBtn) return;
    OtaController::getInstance()->run();
#endif
}

void SyncData::syncTime(){
    Rtc* rtc = Rtc::getInstance();
    Alarm * alarm = Alarm::getInstance();
    OtaController* otaController = OtaController::getInstance();
    if(alarm->getIsRinging() || hasStartBtn) return;
    if(otaController->getState() != OtaController::STATE_CHECK_AVAILABLE &&
       otaController->getState() != OtaController::STATE_RES ) 
        return;
    if( _coutdown_syncTime< 10) {
        _coutdown_syncTime++;
        return;
    }
    std::shared_ptr<HttpRequest> httpRequest = HttpRequest::getInstance();
    String data = httpRequest->get(API_GET_TIME);
    Time time;
    rtc->getTime(time);
    httpRequest->post(String(time.Hour)+":"+String(time.Minute)+" "+String(time.Day)+ "/" +String(time.Month)+ ", Version:" VERSION ", Vol: " + String(getVol())+ "(V)",API_POST,HttpRequest::TYPE_DEBUG);
    if(data.length()> 18){
        
        std::vector<String> listData;
        for(int i =0; i < 6; i++){
            listData.push_back(data.substring(0,2));
            if(i ==4){
                data.remove(0,5);
            }
            else if(i!= 5){
                data.remove(0,3);
            }
        }
        Time oldTime;
        Time current;
        rtc->getTime(oldTime);
        current.Minute = listData.at(1).toInt();
        if(oldTime.Minute !=  current.Minute){
            current.Hour = listData.at(0).toInt();
            current.Second = listData.at(2).toInt();
            current.Day = listData.at(3).toInt();
            current.Month = listData.at(4).toInt();
            current.Year = listData.at(5).toInt();
#if SYNC_TIME_ONLINE
            rtc->setTime(current);
#endif
        }
        removeErrCode(ErrCode::ERR_GET_TIME_FAIL);
    }
    else {
        setErrCode(ErrCode::ERR_GET_TIME_FAIL);
    }
    _coutdown_syncTime=0;
}
void SyncData::syncBugOnline(){
#if SYNC_BUG_ONLINE
    
#endif
}