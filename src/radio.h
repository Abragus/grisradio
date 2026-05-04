#include <RDA5807.h>

class Radio
{
protected:
    RDA5807 rx;

public:
    void setup( uint8_t volume );
    void seekUp();
    void seekDown();
    uint16_t getFrequency();
    void setFrequency(uint16_t frequency);
    uint16_t getDeviceId();
    char * getRdsTime();
    void print();
};