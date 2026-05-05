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
    uint16_t getFrequency();
    void setFrequency(uint16_t frequency);
    uint16_t getDeviceId();
    char * getRdsTime();
    void print();
};