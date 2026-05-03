#pragma once

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

  void setVolume(int8_t vol);
  void changeVolume(int8_t delta);
  void toggleVolumeMute();
  uint8_t getVolume() const;

private:
  GxEPD2_BW<GxEPD2_290_T94_V2, 296U> display = GxEPD2_290_T94_V2(/*CS=*/5, /*DC=*/17, /*RST=*/16, /*BUSY=*/4);

  Container* root = nullptr;
  Container* presetsBox = nullptr;
  Container* infoBox = nullptr;
  Container* volumeBox = nullptr;
  TextElement* frequencyText = nullptr;
  TextElement* stationText = nullptr;
  ShapeElement* volumeLevel = nullptr;
  TaskHandle_t displayWorkerHandle = NULL;
  bool needsRedraw = false;

  uint8_t activePreset = 0;
  float presetFrequencies[4] = {0, 0, 0, 0};
  float frequency = 0.0;
  String stationName = "";
  uint8_t volume = 0;
  uint8_t maxVolume = 15;
  bool volumeMuted = false;

  static void displayWorker(void * param);

  void updateActivePreset();

  void buildLayout();
  void buildPresets();
  void buildInfo();
  void buildVolume();

  void applyInfo();
  void applyVolume();
};