#include "radio.h"

void Radio::setup() {
    rx = RDA5807();
    rx.setup();
    rx.setVolume(1);
    rx.setMono(true);
    rx.setRDS(true);
    rx.setRdsFifo(true);
}

void Radio::seekDown() {
    rx.seek(RDA_SEEK_WRAP, RDA_SEEK_DOWN);
}

void Radio::seekUp() {
    rx.seek(RDA_SEEK_WRAP, RDA_SEEK_UP);
}

uint8_t Radio::getVolume() {
    return rx.getVolume();
}

void Radio::setVolume(uint8_t volume) {
    uint16_t freq = rx.getFrequency();
    this->setup();
    rx.setVolume(volume);
    rx.setFrequency(freq);
}

void Radio::setVolumeUp() {
    rx.setVolumeUp();
}

void Radio::setVolumeDown() {
    rx.setVolumeDown();
}

void Radio::setMute(bool value) {
    rx.setMute(value); 
}

uint16_t Radio::getFrequency() {
    return rx.getFrequency();
}

void Radio::setFrequency(uint16_t frequency) {
    rx.clearRdsFifo();
    rx.clearRdsBuffer();
    rx.setFrequency(frequency);
}

uint16_t Radio::getDeviceId() {
    return rx.getDeviceId();
}

bool Radio::getRdsReady() {
    return rx.getRdsReady();
}

char * Radio::getRdsLocalTime() {
    return rx.getRdsLocalTime();
}

char * Radio::getRdsProgramInformation() {
    return rx.getRdsProgramInformation();
}

char * Radio::getRdsStationInformation() {
    return rx.getRdsStationInformation();
}

char * Radio::getRdsStationName() {
    return rx.getRdsStationName();
}

bool Radio::getRdsAllData(char **stationName, char **stationInformation, char **programInformation, char **utcTime) {
    return rx.getRdsAllData(stationName, stationInformation, programInformation, utcTime);
}


void Radio::print() {
    //Serial.println(rx.getDirectRegister(REG0A).raw);
    //Serial.println(((rda_reg0a *) rx.getStatus(REG0A))->refined.RDSR);
    Serial.println(rx.getFrequency());
    Serial.println(rx.getRealFrequency());
    Serial.println(rx.getRdsReady());
    Serial.println(rx.hasRdsInfo());
    Serial.println(rx.getRdsStationName());
    Serial.println(rx.getVolume());
    Serial.println(rx.isFmTrue());
    //setFrequency(8920);
}