#include "firmware.h"

Firmware::Firmware(){
  
}
Firmware::~Firmware(){

}
int32_t Firmware::isNewFwAvailable(FwInfo& fwInfo){
  HTTPClient http;
  Serial.print("Connecting to server: ");
  Serial.println(API_GET_FW_INFO);
  http.begin(API_GET_FW_INFO);
  int httpCode = http.GET();
  Serial.println("http code" + String (httpCode));
  bool chkData = true;
  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK) {
      String data = http.getString();
      Serial.print("data: ");
      Serial.println(data);
      if(( getValueText(data,"fw_version",fwInfo._fwVersion) > 0 ) && ( getValueInt(data,"fw_size" , fwInfo._totalSize) > 0 )) {
        Serial.println("fw_ver: " + fwInfo._fwVersion);
        Serial.println("fw_size: " + String(fwInfo._totalSize));
        if(fwInfo._fwVersion != VERSION && fwInfo._totalSize > MAXIMUM_FW_LENGTH_SESSION)
        {
          Serial.println("Has fw available \nReset element: ");
          fwInfo._index = 0;
          memset(fwInfo._session.data,0,MAXIMUM_FW_LENGTH_SESSION);
          fwInfo._session._size =0;
          Serial.println("reset done, start updating...");
          return 1;
        }
        else {
          Serial.printf("do not update, fw: %s, total size: %d", fwInfo._fwVersion,fwInfo._totalSize);
          return 0;
        }
      }
    }
  } else {
    Serial.printf("GET request failed, error: %s\n", http.errorToString(httpCode).c_str());
    return -1;
  }
  http.end();
  return -1;
}
uint32_t Firmware::getFwSessionSize(FwInfo& fwInfo){
  if(fwInfo._index >= fwInfo._totalSize) 
    return 0;
  return MAXIMUM_FW_LENGTH_SESSION;
}

int32_t Firmware::downloadFw(const String& url, uint8_t * image,const uint32_t& index, const uint32_t& length){
  HTTPClient http;
  //Serial.println("Connecting to server: " + url);
  http.begin(url);    
  http.addHeader("Range", "bytes=" + String(index) + "-" + String(index + length - 1));
  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_PARTIAL_CONTENT) {
    int sizeImage = http.getSize();
    Serial.printf("Fw size: %d", sizeImage);
    if(sizeImage > MAXIMUM_FW_LENGTH_SESSION) {
      http.end();
      return -1;
    }
    if(sizeImage < 0) {
      http.end();
      return -1;
    }
#if 1
    WiFiClient* stream = http.getStreamPtr();
    int sizeBuff  = stream->available();
    if(sizeImage != sizeBuff) {
      Serial.printf("stream fail, size image: %d, sizeBuff: %d",sizeImage,sizeBuff);
      http.end();
      return -1;
    }
    Serial.printf("is Stream available: %d", sizeImage);
    if(stream->readBytes(image, sizeImage) != sizeImage){
      Serial.println("read byte fail");
      http.end();
      return -1;
    }
#endif
    http.end();
    Serial.printf("remain RAM after HTTP: %d ", 100 * esp_get_free_heap_size() / heap_caps_get_total_size(MALLOC_CAP_8BIT));
    Serial.println("%");
  return sizeImage;
  }
  Serial.printf("http fail, code: %d", httpCode);
  http.end();
  return -1;
}

int32_t Firmware::beginWriteImage(const uint32_t & totalSize) {
  Serial.printf("begin size: %d\n",totalSize);
  return Update.begin(totalSize);
}

int32_t Firmware::writeImage(uint8_t * image, const uint32_t& size ){
  int written = Update.write(image, size);
  Serial.printf("writen length: %d, size input: %d", written, size);
  if(written == size) return (int32_t)written;
  return -1;
}
int32_t Firmware::otaFinish(FwInfo& fwInfo){
  bool status = Update.end();
  Serial.printf("update end: %d",status);
  return status;
}

int32_t Firmware::getValueText(String data, String key, String & value){
  if(data.length() <= 0) return -1;
  json_t mem[MAXIMUM_FW_INFO];
  json_t const* json = json_create( data.begin(), mem, sizeof mem / sizeof *mem );
  if(!json){
    Serial.println("create json text fail");
    return -1;
  }
  json_t const * fw_version = json_getProperty(json, key.c_str());
  if(fw_version && JSON_TEXT == json_getType(fw_version)){
    char const* p_value = json_getValue(fw_version);
       value = String(p_value);
       value.trim();
       Serial.printf("key: %s, value: %s\n", key,value);
      return 1;
    
  }
  Serial.printf("Fail not find out value: %s", key);
  Serial.println("%");
  return 0;
}
int32_t Firmware::getValueInt(String data, String key, int & value){
if(data.length() <= 0) return -1;
  json_t mem[MAXIMUM_FW_INFO];
  json_t const* json = json_create( data.begin(), mem, sizeof mem / sizeof *mem );
  if(!json){
    Serial.println("create json text fail");
    return -1;
  }
  json_t const * fw_version = json_getProperty(json, key.c_str());
  if(fw_version && JSON_INTEGER == json_getType(fw_version)){
    value = json_getInteger(fw_version);
    Serial.printf("key: %s, value: %d\n", key,value);
    return 1;
  }
  Serial.printf("Fail not find out value: %s\n", key);
  return 0;
}