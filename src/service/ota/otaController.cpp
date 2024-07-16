#include "otaController.h"

OtaController::OtaController() {

}
OtaController::~OtaController(){
  
}
int32_t OtaController::isAvailable(){
  return _firmware.isNewFwAvailable(_fwInfo);
}

void OtaController::run(){
  if(isAvailable() <= 0) return;
  if(!_firmware.beginWriteImage(_fwInfo._totalSize)){
    Serial.println("not enough space, update fail");
    return;
  }
  for(_fwInfo._index = 0; _fwInfo._index <= _fwInfo._totalSize; ){
    if(_firmware.getFwSessionSize(_fwInfo) == 0){
      if(!_firmware.otaFinish(_fwInfo)){
        Serial.println("ota fail");
        ESP.restart();
      }
      else {
        Serial.println("ota Success");
        delay(1000);
        ESP.restart();
      }
    }
    for (int i =0; i< 5 ; i++){
      _fwInfo._session._size =_firmware.downloadFw(API_GET_FW_IMAGE, _fwInfo._session.data,_fwInfo._index,MAXIMUM_FW_LENGTH_SESSION);
      if(_fwInfo._session._size > 0) break;
    }
    if((_fwInfo._index+ _fwInfo._session._size) > _fwInfo._totalSize){
      Serial.printf("end status: %d", Update.end());
      Serial.printf("wrong size, size download: %d, total size: %d", (_fwInfo._index+ _fwInfo._session._size),_fwInfo._totalSize);
      ESP.restart();
    }
    if(_firmware.writeImage(_fwInfo._session.data, _fwInfo._session._size) == -1){
      Serial.println("write image fail");
      ESP.restart();
      return;
    }
    _fwInfo._index += _fwInfo._session._size;
    Serial.printf("index: %d,total: %d ",_fwInfo._index,_fwInfo._totalSize);
    Serial.printf("(%d ", _fwInfo._index * 100 / _fwInfo._totalSize);
    Serial.print("%) ");
    Serial.printf("remain RAM: %d", 100 * esp_get_free_heap_size() / heap_caps_get_total_size(MALLOC_CAP_8BIT));
    Serial.println("%");
  }
}
OtaController* OtaController::getInstance(){
  if(!_instance){
    _instance = new OtaController();
  }
  return _instance;
}
OtaController* OtaController::_instance = nullptr;