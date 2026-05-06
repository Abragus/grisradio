#include <RDA5807.h>

class Radio
{
protected:
    RDA5807 rx;

public:
    void setup();
    void seekUp();
    void seekDown();
    uint8_t getVolume();
    void setVolume( uint8_t volume );
    void setVolumeUp();
    void setVolumeDown();
    void setMute( bool value );
    uint16_t getFrequency();
    void setFrequency(uint16_t frequency);
    void setFrequencyUp();
    void setFrequencyDown();
    uint16_t getDeviceId();
    bool getRdsReady();
    char * getRdsLocalTime();
    char * getRdsProgramInformation();
    char * getRdsStationInformation();
    char * getRdsStationName();
    bool getRdsAllData(char **stationName, char **stationInformation, char **programInformation, char **utcTime);
    void print();
};