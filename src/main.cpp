#include <Arduino.h>

#define DEBUG false

#include <GxEPD2_BW.h>
#include <Fonts/FreeSans9pt7b.h>
const GFXfont *font = &FreeSans9pt7b;
#include "GxEPD2_display_selection.h"

#include "alignment.h"
#include "elements.h"

Container channels = Container(Container::HORIZONTAL);
Container infoBox = Container(Container::VERTICAL);
Container volumeBox = Container(Container::HORIZONTAL);

uint8_t channel_selection = 0;
float frequency = 92.8;
String station_name = "Radio Trelleborg";
uint8_t volume = 65;

void setupChannels() {
  const String bottom_texts[] = {"P1", "P2", "P3", "P4"};

  channels.x = 0;
  channels.y = display.height() - 20;
  channels.w = display.width();
  channels.h = 40;
  channels.setMargin(8, 0);

  for (uint8_t i = 0; i < sizeof(bottom_texts) / sizeof(bottom_texts[0]); i++)
  {
    Container* channel_box = new Container();
    channel_box->border = true;
    channel_box->borderRadius = 12;
    channel_box->setMargin(8, 4);
    channel_box->selected = (i + 1 == channel_selection);
    
    TextElement* channel_text = new TextElement(bottom_texts[i], TOP_CENTER);
    channel_box->addChild(channel_text);
    channels.addChild(channel_box);
  }
  channels.arrange();
}
void channelRefresh()
{
  for (uint8_t i = 0; i < channels.children.size(); i++)
  {
    channels.children[i]->selected = (i + 1 == channel_selection);
  }

  channels.updateDisplay(display);
}

void setupInfo()
{
  infoBox.x = channels.children[0]->x;
  infoBox.y = infoBox.x;
  Container* p3 = static_cast<Container*>(channels.children[2]);
  infoBox.w = p3->x + p3->w - infoBox.x;
  infoBox.h = p3->y - infoBox.y - channels.margin_inner;
  infoBox.border = true;
  infoBox.borderRadius = 20;
  infoBox.setMargin(12);

  TextElement* frequencyText = new TextElement(String(frequency, 1) + " MHz", TOP_LEFT);
  TextElement* stationText = new TextElement(station_name, TOP_LEFT);
  infoBox.addChild(frequencyText);
  infoBox.addChild(stationText);

  infoBox.updateDisplay(display);
}

void setupVolume() {
  volumeBox.x = channels.children[3]->x;
  volumeBox.y = infoBox.y;
  volumeBox.border = true;
  volumeBox.borderRadius = 20;
  volumeBox.w = display.width() - volumeBox.x + volumeBox.borderRadius;
  volumeBox.h = infoBox.h;
  volumeBox.setMargin(6);
  volumeBox.childSizes = {1, 2};

  Container* volumeBar = new Container();
  volumeBar->border = true;
  volumeBar->borderRadius = 14;

  ShapeElement* volumeLevel = new ShapeElement();
  volumeLevel->drawFunc = [](Adafruit_GFX& display, int16_t x, int16_t y, uint16_t w, uint16_t h) {
    uint8_t barMargin = 3;
    Size barSize = {(uint16_t)(w - 2 * barMargin), (uint16_t)((h - 2 * barMargin) * volume / 100.0)};
    Point startPoint = alignInsideBox(BOTTOM_CENTER, {x, y, w, h}, barSize, barMargin);
    display.fillRoundRect(startPoint.x, startPoint.y - barMargin, barSize.w, barSize.h, 14 - barMargin, GxEPD_BLACK);
  };

  volumeBar->addChild(volumeLevel);
  volumeBox.addChild(volumeBar);

  Container* volumeIcons = new Container();
  volumeIcons->orientation = Container::VERTICAL;
  volumeIcons->setMargin(0, 0, volumeBox.margin_inner);

  Container* plusButton = new Container();
  plusButton->border = true;
  plusButton->borderRadius = 14;
  plusButton->setMargin(8);
  ShapeElement* plusSymbol = new ShapeElement();
  plusSymbol->drawFunc = [](Adafruit_GFX& display, int16_t x, int16_t y, uint16_t w, uint16_t h) {
    const int16_t lineLength = min(w, h);
    const int8_t lineWidth = 3;
    Frame hLine = {alignInsideBox(MIDDLE_LEFT, {x, y, w, h}, Size(lineLength, lineWidth), 0), Size(lineLength, lineWidth)};
    Frame vLine = {alignInsideBox(MIDDLE_CENTER, hLine, Size(lineWidth, lineLength), 0), Size(lineWidth, lineLength)};
    display.fillRoundRect(hLine.x, hLine.y, hLine.w, hLine.h, lineWidth, GxEPD_BLACK);
    display.fillRoundRect(vLine.x, vLine.y, vLine.w, vLine.h, lineWidth, GxEPD_BLACK);
  };
  plusButton->addChild(plusSymbol);
  volumeIcons->addChild(plusButton);

  Container* minusButton = new Container();
  minusButton->border = true;
  minusButton->borderRadius = 14;
  minusButton->setMargin(8);
  ShapeElement* minusSymbol = new ShapeElement();
  minusSymbol->drawFunc = [](Adafruit_GFX& display, int16_t x, int16_t y, uint16_t w, uint16_t h) {
    const int16_t lineLength = min(w, h);
    const int8_t lineWidth = 3;
    Frame hLine = {alignInsideBox(MIDDLE_LEFT, {x, y, w, h}, Size(lineLength, lineWidth), 0), Size(lineLength, lineWidth)};
    display.fillRoundRect(hLine.x, hLine.y, lineLength, lineWidth, lineWidth, GxEPD_BLACK);
  };
  minusButton->addChild(minusSymbol);
  volumeIcons->addChild(minusButton);
  volumeBox.addChild(volumeIcons);

  volumeBox.updateDisplay(display);
}

void setup()
{
  display.init(115200, true, 2, false);
  display.setRotation(3);
  display.setFont(font);
  display.setTextColor(GxEPD_BLACK);

  if (DEBUG)
  {
    Serial.begin(115200);
    while (!Serial) ;
    Serial.println("DEBUG enabled: serial timing logs on");
  }

  setupChannels();
  setupInfo();
  setupVolume();

  do
  {
    channelRefresh();
    channel_selection = channel_selection % 5 + 1;
  } while (channel_selection <= 5);

  display.hibernate();
}

void loop() {};
