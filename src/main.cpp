#include <Arduino.h>
#include "ButtonManager.h"
#include "gui.h"

#define DEBUG false

GUI gui;
ButtonManager buttonManager;

void setup() {
  if (DEBUG) {
    Serial.begin(115200);
    while (!Serial);
    Serial.println("DEBUG enabled");
  }
  
  gui.begin();
  gui.setPresetFrequency(1, 92.8);
  gui.activatePreset(1);
  gui.setStationName("Radio Trelleborg");
  gui.setVolume(10);

  // PinID, Callback
  buttonManager.actions = {
    {{13, AceButton::kEventClicked}, []() { gui.activatePreset((gui.getActivePreset() % 4) + 1);}}, // Testing: Cycle through presets
    {{13, AceButton::kEventLongPressed}, []() { gui.setPresetFrequency(3); }}, // Testing
    {{14, AceButton::kEventPressed}, []() { gui.changeVolume(1); }},
    {{14, AceButton::kEventRepeatPressed}, []() { gui.changeVolume(1); }},
    {{12, AceButton::kEventPressed}, []() { gui.changeVolume(-1); }},
    {{12, AceButton::kEventRepeatPressed}, []() { gui.changeVolume(-1); }},
    {{0, AceButton::kEventClicked}, []() { gui.toggleVolumeMute(); }},
    {{0, AceButton::kEventPressed}, []() { gui.changeFrequency(0.1); }},
    {{0, AceButton::kEventRepeatPressed}, []() { gui.changeFrequency(0.1); }},
    {{0, AceButton::kEventPressed}, []() { gui.changeFrequency(-0.1); }},
    {{0, AceButton::kEventRepeatPressed}, []() { gui.changeFrequency(-0.1); }},
    {{0, AceButton::kEventClicked}, []() { gui.activatePreset(1); }},
    {{0, AceButton::kEventClicked}, []() { gui.activatePreset(2); }},
    {{0, AceButton::kEventClicked}, []() { gui.activatePreset(3); }},
    {{0, AceButton::kEventClicked}, []() { gui.activatePreset(4); }},
    {{0, AceButton::kEventLongPressed}, []() { gui.setPresetFrequency(1); }},
    {{0, AceButton::kEventLongPressed}, []() { gui.setPresetFrequency(2); }},
    {{0, AceButton::kEventLongPressed}, []() { gui.setPresetFrequency(3); }},
    {{0, AceButton::kEventLongPressed}, []() { gui.setPresetFrequency(4); }},
    {{0, AceButton::kEventClicked}, []() { }} // Power button
  };

  buttonManager.setup();
}

void loop() {
  buttonManager.loop();
}