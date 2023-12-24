#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "board/io.h"
#include "board/uart.h"
#define TIMEOUT_SENSOR 60000
#define TIME_CHECKING_SENSOR 700
class ReadSensor{
public:
    struct DataSensor{
        uint8_t round;
        uint8_t angle;
    };
    void init(){
        uartSensor.begin();
    }
    uint8_t readRound(){
        uint8_t _round = this->dataSensor.round;
        this->dataSensor.round=0;
        return _round;
    }
    void clearRound(){
        this->dataSensor.round =0;
    }
    void threadJoin(){
        for(std::thread& thread :threads){
            if(thread.joinable())
                thread.join();
        }
    }
    static std::shared_ptr<ReadSensor> getInstance(){
        if(!readSensor){
            readSensor = std::shared_ptr<ReadSensor>(new ReadSensor());
        }
        return readSensor;
    }
private:
    bool hasSignalSensor;
    bool enableLed;
    static std::shared_ptr<ReadSensor> readSensor;
    std::mutex mtx;
    std::condition_variable cv;
    uint64_t timeoutSensor;
    DataSensor dataSensor;
    std::vector<std::thread> threads;
    void process3Wire(){
        while (1)
        {
        if(signalSensor3Wire){
            this->dataSensor.round++;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }
    void process4wire(){
        while (1)
        {   
            String data = "";
            if((millis() - timeoutSensor) > TIMEOUT_SENSOR){
                setErrCode(ErrCode::ERR_SENSOR_FAIL);
            }
            if(!uartSensor.read(data)){
                continue;
            }
            if(data != "*"){
                dataSensor.angle = data.toInt();
                dataSensor.round++;
            }
            timeoutSensor = millis();
        }
        
    }
    void processBlinkLed(){
        while (1)
        {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [&] { return this->enableLed; });
        setHigh(LED_SIGNAL);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        setLow(LED_SIGNAL);
        this->enableLed = false;
        }
        
    }
    ReadSensor(): hasSignalSensor(false), dataSensor({0,0}),enableLed(false){
        threads.push_back(std::thread(&ReadSensor::process3Wire, this));
        threads.push_back(std::thread(&ReadSensor::process4wire,this));
        threads.push_back(std::thread(&ReadSensor::processBlinkLed,this));
    }

};
std::shared_ptr<ReadSensor> ReadSensor::readSensor = nullptr;