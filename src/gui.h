#pragma once

#include "pinin.h" 
#include <SPI.h>
#include <GxEPD2_BW.h>
#include <Fonts/FreeSans9pt7b.h>
static const GFXfont *font = &FreeSans9pt7b;
#include "elements.h"
#include "alignment.h"

class GUI {
public:
  GUI();
  ~GUI();

  void begin();
  void draw();

  void setFrequency(float freq);
  void changeFrequency(float delta);
  float getFrequency() const;

  bool setPresetFrequency(uint8_t preset, float freq);
  bool setPresetFrequency(uint8_t preset);
  float getPresetFrequency(uint8_t preset) const;
  float activatePreset(uint8_t preset);
  uint8_t getActivePreset() const;

  void setStationName(const String& name);
  String getStationName() const;
  
  void setTime(const String& time);
  String getTime() const;

  void setVolume(int8_t vol);
  void changeVolume(int8_t delta);
  void setVolumeMute(bool mute);
  void toggleVolumeMute();
  bool isVolumeMuted() const;
  uint8_t getVolume() const;

  void setBatteryLevel(float level);
  uint8_t getBatteryLevel() const;

private:
  GxEPD2_BW<GxEPD2_290_T94_V2, 296U> display = GxEPD2_290_T94_V2(/*CS=*/DISPLAY_CS, /*DC=*/DISPLAY_DC, /*RST=*/DISPLAY_RST, /*BUSY=*/DISPLAY_BUSY);

  Container* root = nullptr;
  Container* presetsBox = nullptr;
  Container* infoBox = nullptr;
  Container* topBar = nullptr;
  Container* volumeBox = nullptr;
  TextElement* frequencyText = nullptr;
  TextElement* timeText = nullptr;
  TextElement* stationText = nullptr;
  ShapeElement* volumeLevel = nullptr;

  ShapeElement* batteryIcon = nullptr;
  TaskHandle_t displayWorkerHandle = NULL;
  bool needsRedraw = false;

  uint8_t activePreset = 0;
  float presetFrequencies[4] = {0, 0, 0, 0};
  float frequency = 0.0;
  String stationName = "";
  String time = "";
  uint8_t volume = 0;
  uint8_t maxVolume = 15;
  bool volumeMuted = false;
  int8_t batteryLevel = -1;

  static void displayWorker(void * param);

  void updateActivePreset();

  void buildLayout();
  void buildPresets();
  void buildInfo();
  void buildVolume();

  void applyInfo();
  void applyVolume();
};