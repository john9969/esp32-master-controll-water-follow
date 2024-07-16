#include "Log.h"

Log::Log(){
}

Log::~Log(){

}

int32_t Log::addLog(const String& data, TYPE_LOG typeLog){
    Rtc* rtc = Rtc::getInstance();
    
    if(this->_logData.isEmpty()) {
        this->_logData = data;
    }
    return -1;
}
int32_t Log::getLengthLogData() const{
    return 0;
}
String Log::getLogData() const{
 return "";
}
int32_t Log::clearLogData(){
    return 0;
}