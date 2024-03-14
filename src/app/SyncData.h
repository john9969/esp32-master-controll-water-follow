#include "logic_controll.h"
#include "board/read_adc.h"
class SyncData{
    public:
    void syncErr();
    void syncTime();
    private:
    int coutdown_update = 0;
    
};
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
void SyncData::syncTime(){
    Rtc* rtc = Rtc::getInstance();
    Alarm * alarm = Alarm::getInstance();
    if(alarm->getIsRinging() || hasStartBtn) return;
    if(coutdown_update < 12) {
        coutdown_update++;
        return;
    }
    //String data;
    std::shared_ptr<HttpRequest> httpRequest = HttpRequest::getInstance();
    String data = httpRequest->get(API_GET_TIME);
    Time time;
    rtc->getTime(time);
    httpRequest->post(String(time.Hour)+":"+String(time.Minute)+" "+String(time.Day)+ "/" +String(time.Month)+",Vol" + String(getVol()),API_POST,HttpRequest::TYPE_DEBUG);
    if(data.length()> 18){
        
        std::vector<String> listData;
        for(int i =0; i < 6; i++){
            listData.push_back(data.substring(0,2));
            //Serial.println(listData.at(i));
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
            rtc->setTime(current);
        }
        removeErrCode(ErrCode::ERR_GET_TIME_FAIL);
    }
    else {
        setErrCode(ErrCode::ERR_GET_TIME_FAIL);
    }
    coutdown_update=0;
}