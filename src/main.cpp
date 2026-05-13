#include <Arduino.h>
#include <deque>
#include "ButtonManager.h"
#include "gui.h"
#include "radio.h"

#define DEBUG false
#define FIFOQueueLength 15

GUI gui;
ButtonManager buttonManager;
Radio radio;

String currentStationName, currentProgramInformation, localTime, newLocalTime;
std::deque<String> programInformation, stationName;
bool rdsUpdate;
uint16_t battery, newBattery;

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

void clearRDS() {
  stationName.clear();
  currentStationName = "";
  programInformation.clear();
  currentProgramInformation = "";

  gui.setStationName("");
}

void setFrequency(uint16_t frequency) {
  radio.setFrequency(frequency);
  gui.setFrequency(frequency/100.0);
  clearRDS();
}

void setFrequencyUp() {
  radio.setFrequencyUp();
  gui.setFrequency(radio.getFrequency()/100.0);
  clearRDS();
}

void setFrequencyDown() {
  radio.setFrequencyDown();
  gui.setFrequency(radio.getFrequency()/100.0);
  clearRDS();
}

void activatePreset(uint8_t preset) {
  clearRDS();
  radio.setFrequency((uint16_t) (gui.activatePreset(preset) * 100));
}

void seekDown() {
  clearRDS();
  radio.seekDown();
  gui.setFrequency(radio.getFrequency()/100);
}

void seekUp() {
  clearRDS();
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

static const char allowedExtraChars[] = " åäöÅÄÖ";
static const char toleratedSpecialChars[] = ".,!?&-'():$/";

void filterRDSText(char* str) {
  if (str == nullptr) return;

  char* src = str;
  char* dst = str;

  while (*src) {
    char c = *src;

    // Check if character is in whitelist
    if (isalnum((unsigned char)c) || strchr(toleratedSpecialChars, c) || strchr(allowedExtraChars, c)) {
      *dst++ = c;
    }
    src++;
  }
      
  // Trim trailing space and null-terminate
  if (dst > str && *(dst - 1) == ' ') dst--;
  *dst = '\0';
}

char* cleanString(char* s) {
  filterRDSText(s);
  return s;
}

String handleRDSinfo(std::deque<String> * queue, String newString) {
  queue->push_front(String(cleanString((char*) newString.c_str())));
  if (queue->front() == "") {
    Serial.println("Empty RDS info received, skipping...");
    queue->pop_front();
    return "";
  }
  String bestString = "";
  
  Serial.println("New RDS info: '" + newString + "'");
  if(queue->size() > FIFOQueueLength) {
    queue->pop_back();
  }

  int8_t highestScore = -1;
  // Give points to each string, according to:
  // Length + Occurences in the queue - Number of special chars (toleratedSpecialChars)
  for (String str : *queue) {
    int8_t score = str.length() + std::count(queue->begin(), queue->end(), str);
    for (char c : toleratedSpecialChars) {
      score -= std::count(str.begin(), str.end(), c);
    }

    if(score > highestScore) {
      highestScore = score;
      bestString = str;
    }
  }
  Serial.println("Best RDS string: '" + bestString + "' with score " + String(highestScore) + ", occurences: " + String(std::count(queue->begin(), queue->end(), bestString)));

  return bestString;
}

void setup() {
  if (DEBUG) {
    Serial.begin(115200);
    while (!Serial);
    Serial.println("DEBUG enabled");
  }
  
  gui.begin();
  delay(250);
  radio.setup();
  gui.setVolume(radio.getVolume());
  gui.setPresetFrequency(1, 88.8);
  gui.setPresetFrequency(2, 107);
  gui.setPresetFrequency(3, 97.0);
  gui.setPresetFrequency(4, 101.4);
  activatePreset(1);

  // PinID, Callback
  buttonManager.actions = {
    {{VOLUME_UP, AceButton::kEventPressed}, []() { setVolumeUp(); }},
    {{VOLUME_UP, AceButton::kEventRepeatPressed}, []() { setVolumeUp(); }},
    {{VOLUME_DOWN, AceButton::kEventPressed}, []() { setVolumeDown(); }},
    {{VOLUME_DOWN, AceButton::kEventRepeatPressed}, []() { setVolumeDown(); }},
    {{VOLUME_MUTE, AceButton::kEventClicked}, []() { toggleMute(); }},
    {{FREQUENCY_UP, AceButton::kEventPressed}, []() { setFrequencyUp(); }},
    {{FREQUENCY_UP, AceButton::kEventRepeatPressed}, []() { setFrequencyUp(); }},
    {{FREQUENCY_DOWN, AceButton::kEventPressed}, []() { setFrequencyDown(); }},
    {{FREQUENCY_DOWN, AceButton::kEventRepeatPressed}, []() { setFrequencyDown(); }},
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
    newBattery = 0;

    for (size_t i = 0; i < 20; i++) {
      newBattery += analogRead(A13);
    }

    newBattery = newBattery/10; // newBattery/20 * 2
    
    if (newBattery < 4219) {
      newBattery = map(newBattery, 3599, 4219, 0, 20);
    } else {
      newBattery = map(newBattery, 4219, 5130, 20, 100);
    }

    Serial.println(radio.getRdsReady() ? "\n\nRDS ready. " : "\n\nRDS not ready. ");
    String newStationName = handleRDSinfo(&stationName, radio.getRdsStationName());
    if (newStationName.length() > 0 && newStationName != currentStationName) {
      currentStationName = newStationName;
      rdsUpdate = true;
    }

    String newProgramInformation = handleRDSinfo(&programInformation, radio.getRdsProgramInformation());
    if (newProgramInformation.length() > 0 && newProgramInformation != currentProgramInformation) {
      currentProgramInformation = newProgramInformation;
      rdsUpdate = true;
    }

    if(newLocalTime != localTime && localTime != "") {
      localTime = newLocalTime;
      gui.setTime(localTime);
    }
    
    if(newBattery != battery) {
      battery = newBattery;
      gui.setBatteryLevel(battery);
    }

    if(rdsUpdate) {
      gui.setStationName(currentStationName + "\n" + currentProgramInformation);
      rdsUpdate = false;
    }

    prev = millis();
  }
}