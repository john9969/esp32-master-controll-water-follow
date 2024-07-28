#include "otaController.h"
#include "../connection/Connection.h"
#include "../../app/logicControl/LoginControl.h"
OtaController::OtaController() : _state(STATE::STATE_RES),_heartBeat(DEFAULT_HEART_BEAT)
,_timeoutCheckNewFw()
{

}
OtaController::~OtaController(){
  
}
int32_t OtaController::isAvailable(){
  return _firmware.isNewFwAvailable(_fwInfo);
}

void OtaController::run(){
  if((!Connection::getInstance()->isConnected())  &&
     (getState() != STATE_CHECK_AVAILABLE)        &&
     (getState() != STATE_RES)                    &&
     (--_heartBeat == 0 )) {
      setState(STATE_FAIL);
      return;
  }
  if(LogicControl::getInstance()->getState() != LogicControl::STATE_RES){
    return;
  }
  switch (getState())
  {
  case STATE_CHECK_AVAILABLE:
    if(isAvailable() > 0) {
      setState(STATE_INIT);
      break;
    }
    setState(STATE_RES);
    _timeoutCheckNewFw = TIMEOUT_CHECK_NEW_FW;
    break;
  case STATE_INIT:
    if(!_firmware.beginWriteImage(_fwInfo._totalSize)){
      Serial.println("not enough space, update fail");
      setState(STATE_FAIL);
      break;
    }
    _fwInfo._index = 0;
    Serial.printf("begin size: %d\n",_fwInfo._totalSize);
    setState(STATE_DOWNLOAD);
    break;
  case STATE_DOWNLOAD:
    if(!_firmware.getFwSessionSize(_fwInfo)){
      setState(STATE_FINISH);
      break;
    }
    for (int i =0; i< 5 ; i++){
      _fwInfo._session._size =_firmware.downloadFw(API_GET_FW_IMAGE, _fwInfo._session.data,_fwInfo._index,MAXIMUM_FW_LENGTH_SESSION);
      if(_fwInfo._session._size > 0) break;
    }
    if((_fwInfo._index + _fwInfo._session._size) > _fwInfo._totalSize){
      Serial.printf("end status: %d", Update.end());
      Serial.printf("wrong size, size download: %d, total size: %d", (_fwInfo._index+ _fwInfo._session._size),_fwInfo._totalSize);
      ESP.restart();
    }
    setState(STATE_UPLOAD);
    break;
  case STATE_UPLOAD:
    if(_firmware.writeImage(_fwInfo._session.data, _fwInfo._session._size) == -1){
      Serial.println("write image fail");
      ESP.restart();
      return;
    }
    _fwInfo._index += _fwInfo._session._size;
    Lcd::getInstance()->setCursor(0,3);
    Lcd::getInstance()->print(String(_fwInfo._index)+ "/" + String(_fwInfo._totalSize) + "(" + String(_fwInfo._index * 100 / _fwInfo._totalSize) + "%)");
    Serial.printf("index: %d,total: %d ",_fwInfo._index,_fwInfo._totalSize);
    Serial.printf("(%d ", _fwInfo._index * 100 / _fwInfo._totalSize);
    Serial.println("%)");
    setState(STATE_DOWNLOAD);
    break;
  case STATE_FINISH:
    if(!_firmware.otaFinish(_fwInfo)){
      Serial.println("ota fail");
      ESP.restart();
    }
    else {
      Serial.println("ota Success");
      delay(1000);
      ESP.restart();
    }
    break;
  case STATE_FAIL:
    break;
  case STATE_RES:
    if(--_timeoutCheckNewFw > 0) break;
    setState(STATE_CHECK_AVAILABLE);
    break;
  }

}
OtaController* OtaController::getInstance(){
  if(!_instance){
    _instance = new OtaController();
  }
  return _instance;
}
OtaController::STATE OtaController::getState() const {
  return this->_state;
}
void OtaController::setState(const STATE& state){
  switch (state)
  {
  case STATE_INIT:
    Serial.println("ota, state init");
    break;
  case STATE_DOWNLOAD:
  Serial.println("ota, state download");
    if(_heartBeat != DEFAULT_HEART_BEAT) _heartBeat = DEFAULT_HEART_BEAT;
    break;
  case STATE_UPLOAD:
  Serial.println("ota, state upload");
    if(_heartBeat != DEFAULT_HEART_BEAT) _heartBeat = DEFAULT_HEART_BEAT;
    break;
  case STATE_FINISH:
  Serial.println("ota, state finish");
    if(_heartBeat != DEFAULT_HEART_BEAT) _heartBeat = DEFAULT_HEART_BEAT;
    break;
  case STATE_FAIL:
  Serial.println("ota, state fail");
    ESP.restart();
    break;
  case STATE_RES:
    break;
  }
  this->_state = state;
  _heartBeat = DEFAULT_HEART_BEAT;
}
OtaController* OtaController::_instance = nullptr;