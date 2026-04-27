#include "Radio.h"

void Radio::setup( uint8_t volume ) {
    rx = RDA5807();
    rx.setup();
    rx.setMono(true);
    rx.setVolume(volume);
    rx.setRDS(true);
    rx.setRdsFifo(true);
}

void Radio::seekDown() {
    rx.seek(RDA_SEEK_WRAP, RDA_SEEK_DOWN);
}

void Radio::seekUp() {
    rx.seek(RDA_SEEK_WRAP, RDA_SEEK_UP);
}

uint16_t Radio::getFrequency() {
    return rx.getFrequency();
}

void Radio::setFrequency(uint16_t frequency) {
    rx.setFrequency(frequency);
}

uint16_t Radio::getDeviceId() {
    return rx.getDeviceId();
}

char * Radio::getRdsTime() {
    if (!rx.getRdsReady() or !rx.hasRdsInfo()) {
        return "RDS not ready";
    }

    return rx.getRdsTime();
}

void Radio::print() {
    Serial.println(rx.getDirectRegister(REG0A).raw);
    Serial.println(((rda_reg0a *) rx.getStatus(REG0A))->refined.RDSR);
    Serial.println(rx.getRdsReady());
    sleep(10);
}

