#ifndef GUI_H
#define GUI_H

#include <GxEPD2_BW.h>
#include <Fonts/FreeSans9pt7b.h>
static const GFXfont *font = &FreeSans9pt7b;
// #include "GxEPD2_display_selection.h"
#include "elements.h"
#include "alignment.h"

// extern GxEPD2_BW<GxEPD2_290_T94_V2, 296U> display;

class GUI {
public:
  GUI();
  ~GUI();

  void begin();
  void draw();

  void setChannel(uint8_t channel);
  uint8_t getChannel() const;

  void setFrequency(float freq);
  void setStationName(const String& name);
  void setVolume(uint8_t vol);

private:
  GxEPD2_BW<GxEPD2_290_T94_V2, 296U> display = GxEPD2_290_T94_V2(/*CS=*/5, /*DC=*/17, /*RST=*/16, /*BUSY=*/4);

  Container* root = nullptr;
  Container* channels = nullptr;
  Container* infoBox = nullptr;
  Container* volumeBox = nullptr;
  TextElement* frequencyText = nullptr;
  TextElement* stationText = nullptr;
  ShapeElement* volumeLevel = nullptr;

  uint8_t channelSelection = 1;
  float frequency = 92.8;
  String stationName = "Radio Trelleborg";
  uint8_t volume = 65;

  void buildLayout();
  void buildChannels();
  void buildInfo();
  void buildVolume();

  void applyChannelSelection();
  void applyInfo();
  void applyVolume();
};

#endif // GUI_H
