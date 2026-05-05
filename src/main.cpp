#include <Arduino.h>
#include <deque>
#include "ButtonManager.h"
#include "gui.h"
#include "radio.h"

#define DEBUG true
#define FIFOQueueLength 10

GUI gui;
ButtonManager buttonManager;
Radio radio;

  String bestStationName, lastStationName, bestProgramInformation, lastProgramInformation, localTime, newLocalTime;
  std::deque<String> programInformation, stationName;
  bool rdsUpdate;

void setVolume(uint8_t volume) {
  if (volume > 15) {
    volume = 15;
  }
  
  radio.setMute(false);
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
  stationName.clear();
  programInformation.clear();
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

void toggleMute() {
  gui.toggleVolumeMute();
  radio.setMute(gui.isVolumeMuted());
  if(!gui.isVolumeMuted()) {
    radio.setVolume(gui.getVolume());
  }
}

void filterRDSText(char* str) {
  if (str == nullptr) return;

  char* src = str;
  char* dst = str;

  while (*src) {
    char c = *src;

    // Check if character is in our "Keep" whitelist
    if ((c >= 'A' && c <= 'Z') ||       // Uppercase
        (c >= 'a' && c <= 'z') ||       // Lowercase
        (c >= '0' && c <= '9') ||       // Numbers
        (c == ' ')  || (c == '.')  ||   // Space and period
        (c == ',')  || (c == '!')  ||   // Comma and bang
        (c == '?')  || (c == '&')  ||   // Question and ampersand
        (c == '-')  || (c == '\'') ||   // Dash and apostrophe
        (c == '(')  || (c == ')')  ||   // Parentheses
        (c == ':')  ||   // Colons
        (c == '$')  ||  // Symbols
        (c == '/'))     // Math/Paths
    {
      *dst++ = c;
    }
    src++;
  }
  *dst = '\0'; // Properly terminate the filtered string
}

char* cleanString(char* s) {
  filterRDSText(s);
  return s;
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

  // PinID, Callback
  buttonManager.actions = {
    {{VOLUME_UP, AceButton::kEventPressed}, []() { setVolumeUp(); }},
    {{VOLUME_UP, AceButton::kEventRepeatPressed}, []() { setVolumeUp(); }},
    {{VOLUME_DOWN, AceButton::kEventPressed}, []() { setVolumeDown(); }},
    {{VOLUME_DOWN, AceButton::kEventRepeatPressed}, []() { setVolumeDown(); }},
    {{VOLUME_MUTE, AceButton::kEventClicked}, []() { toggleMute(); }},
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
  };

  buttonManager.setup();
}

  long prev = millis();

void loop() {
  buttonManager.loop();
  if (millis() > prev + 1000) {
    newLocalTime = radio.getRdsLocalTime();
    bestProgramInformation = "";
    bestStationName = "";

    programInformation.push_front(cleanString(radio.getRdsProgramInformation()));
    if(programInformation.size() > FIFOQueueLength) {
      programInformation.pop_back();
    }

    for(String progInfo : programInformation) {
      if(progInfo.length() > bestProgramInformation.length()) {
        bestProgramInformation = progInfo;
      }
    }

    if(bestProgramInformation != lastProgramInformation) {rdsUpdate = true;}

    stationName.push_front(cleanString(radio.getRdsStationName()));
    if(stationName.size() > FIFOQueueLength) {
      stationName.pop_back();
    }

    for(String statName : stationName) {
      if(statName.length() > bestStationName.length()) {
        bestStationName = statName;
      }
    }

    if(bestStationName != lastStationName) {rdsUpdate = true;}

    if(newLocalTime != localTime && localTime != "") {
      localTime = newLocalTime;
      gui.setTime(localTime);
    }

    if(rdsUpdate) {
      lastStationName = bestStationName;
      lastProgramInformation = bestProgramInformation;
      Serial.println(bestStationName + "\n" + bestProgramInformation);
      gui.setStationName(bestStationName + "\n" + bestProgramInformation);
      rdsUpdate = false;
    }

    prev = millis();
  }
}