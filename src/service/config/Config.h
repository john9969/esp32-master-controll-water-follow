#ifndef SERVICE_HTTPREQUEST_H
#define SERVICE_HTTPREQUEST_H
#include <iostream>
#include <list>
#define SERIAL_NUMBER "111"
#define LOWER_VOL_THRESHOLD 22
struct DataConfig
{
    std::string _sn;
    int _fre;
    int  _lowerVolThreashold;

    DataConfig operator=(DataConfig const& obj){
        DataConfig p_config;
        p_config._sn = obj._sn;
        p_config._fre = obj._fre;
        p_config._lowerVolThreashold = obj._lowerVolThreashold;
        return p_config;
    };
};

class Config{
    public:
    virtual std::string getSn() = 0;
    virtual void hasFreChange(const int & fre ) = 0;
};
class UpdateConfig : public Config{
public:
    UpdateConfig() : _config({SERIAL_NUMBER, 1, LOWER_VOL_THRESHOLD}){
        
    }
    ~UpdateConfig();
private:
    DataConfig _config;
}


#endif