#include "HttpRequest.h"

std::shared_ptr<HttpRequest> HttpRequest::_httpRequest = nullptr;
HttpRequest::HttpRequest(){

}
std::shared_ptr<HttpRequest> HttpRequest::getInstance(){
    if(!_httpRequest){
        _httpRequest = std::shared_ptr<HttpRequest>(new HttpRequest());
    }
    return _httpRequest;
}
String HttpRequest::post(const std::vector<std::vector<String>>& data, const String& api){
    Connection* connection = Connection::getInstance();
    DataConfig * config = DataConfig::getInstance();
    if(!connection->isConnected()){
        setErrCode(ERR_WIFI_LOST_CONNECT);
        return "";
    }
    std::unique_ptr<HTTPClient> http(new HTTPClient());
    http->begin(api.c_str());    
    http->addHeader("Content-Type", "application/x-www-form-urlencoded");
    //String httpRequestData = "apikey=senddata&keyword=TA&workstationid=010&data=100'120'180' 060'090'090'";
    String dataPost ="apikey=senddata"
                 "&keyword=";   
    dataPost += "TA";
    dataPost += "&workstationid=" + config->getSerialNumber();
    dataPost += "&data=";
    for(std::vector<String> value:data){
        for(String p_value : value){
            dataPost += p_value;
        }
    }
    Serial.println("data post: "  + dataPost);  
    
    int httpResponseCode = http->POST(dataPost);
    String payload= "";
    if (httpResponseCode>0) {
        payload= http->getString();
        Serial.print("HTTP Payload: " + payload);
    }
    else {
        Serial.print("HTTP Payload: " + http->getString());
    }
    http->end();
    return payload;
}
String HttpRequest::post(const String& data, const String& api, const Type& type){
    Connection* connection = Connection::getInstance();
    DataConfig* config = DataConfig::getInstance();
    if(!connection->isConnected()){
        setErrCode(ERR_WIFI_LOST_CONNECT);
        return "";
    }
    std::unique_ptr<HTTPClient> http(new HTTPClient());
    http->begin(api.c_str());    
    http->addHeader("Content-Type", "application/x-www-form-urlencoded");
    String dataPost =   "apikey=senddata"
                        "&keyword=";   
    if(type == TYPE_CRITICAL) dataPost +=       "TC";
    else if( type == TYPE_DEBUG) dataPost +=    "TB";
    dataPost += "&workstationid=" + config->getSerialNumber();
    dataPost += "&data=";
    dataPost += data;
    Serial.println("data post" + dataPost);  
    int httpResponseCode = http->POST(dataPost);
    String payload= "";
    if (httpResponseCode>0) {
        payload= http->getString();
        Serial.print("HTTP Payload: " + payload);
    }
    else {
        Serial.print("HTTP Payload: " + http->getString());
    }
    http->end();
    return payload;
}
String HttpRequest::get(const String& api){
    Connection* connection = Connection::getInstance();
    if(!connection->isConnected()){
        setErrCode(ERR_WIFI_LOST_CONNECT);
        return "";
    }
    std::unique_ptr<HTTPClient> http(new HTTPClient());
    http->begin(api.c_str());    
    int httpResponseCode = http->GET();

    String payload= "";
    if (httpResponseCode>0) {
        payload= http->getString();
        Serial.print("HTTP Payload: " + payload);
    }
    else {
        Serial.print("HTTP Payload: " + http->getString());
    }
  http->end();
  return payload;      
}
