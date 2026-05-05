#include <Arduino.h>
#include "ButtonManager.h"
#include "gui.h"
#include "radio.h"

#define DEBUG true

GUI gui;
ButtonManager buttonManager;
Radio radio;

void setVolume(uint8_t volume) {
  if (volume > 15) {
    volume = 15;
  }
  
  radio.setVolume(volume);
  gui.setVolume(volume);
}

void setVolumeUp() {
  gui.changeVolume(1);
  radio.setVolume(gui.getVolume());
}

void setVolumeDown() {
  gui.changeVolume(-1);
  radio.setVolume(gui.getVolume());
}

void setFrequency(uint16_t frequency) {
  radio.setFrequency(frequency);
  gui.setFrequency(frequency/100);
}

void activatePreset(uint8_t preset) {
  radio.setFrequency((uint16_t) (gui.activatePreset(preset) * 100));
}

void seekDown() {
  radio.seekDown();
  gui.setFrequency(radio.getFrequency()/100);
}

void seekUp() {
  radio.seekUp();
  gui.setFrequency(radio.getFrequency()/100);
}
void setup() {
  if (DEBUG) {
    Serial.begin(115200);
    while (!Serial);
    Serial.println("DEBUG enabled");
  }
  
  gui.begin();
  radio.setup();
  gui.setVolume(radio.getVolume());
  gui.setPresetFrequency(1, 101.4);
  gui.setPresetFrequency(2, 88.8);
  gui.setPresetFrequency(3, 107);
  gui.setPresetFrequency(4, 87.9);
  activatePreset(3);
  gui.setStationName("Radio Trelleborg");

  // PinID, Callback
  buttonManager.actions = {
    {{0, AceButton::kEventClicked}, []() { gui.activatePreset((gui.getActivePreset() % 4) + 1);}}, // Testing: Cycle through presets
    {{0, AceButton::kEventLongPressed}, []() { gui.setPresetFrequency(3); }}, // Testing
    {{VOLUME_UP, AceButton::kEventPressed}, []() { setVolumeUp(); }},
    {{VOLUME_UP, AceButton::kEventRepeatPressed}, []() { setVolumeUp(); }},
    {{VOLUME_DOWN, AceButton::kEventPressed}, []() { setVolumeDown(); }},
    {{VOLUME_DOWN, AceButton::kEventRepeatPressed}, []() { setVolumeDown(); }},
    {{VOLUME_MUTE, AceButton::kEventClicked}, []() { gui.toggleVolumeMute(); }},
    {{FREQUENCY_UP, AceButton::kEventPressed}, []() { seekUp(); }},
    {{FREQUENCY_DOWN, AceButton::kEventPressed}, []() { seekDown(); }},
    {{PRESET_1, AceButton::kEventClicked}, []() { activatePreset(1); }},
    {{PRESET_2, AceButton::kEventClicked}, []() { activatePreset(2); }},
    {{PRESET_3, AceButton::kEventClicked}, []() { activatePreset(3); }},
    {{PRESET_4, AceButton::kEventClicked}, []() { activatePreset(4); }},
    {{PRESET_1, AceButton::kEventLongPressed}, []() { gui.setPresetFrequency(1); }},
    {{PRESET_2, AceButton::kEventLongPressed}, []() { gui.setPresetFrequency(2); }},
    {{PRESET_3, AceButton::kEventLongPressed}, []() { gui.setPresetFrequency(3); }},
    {{PRESET_4, AceButton::kEventLongPressed}, []() { gui.setPresetFrequency(4); }},
    {{0, AceButton::kEventClicked}, []() { }} // Power button
  };

  buttonManager.setup();
}

  long prev = millis();

void loop() {
  buttonManager.loop();
  if (millis() > prev + 1000) {
    radio.print();
    prev = millis();
  }
  //activatePreset((gui.getActivePreset() % 4) + 1);
}