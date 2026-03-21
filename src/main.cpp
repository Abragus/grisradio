#include <Arduino.h>

#define DEBUG false

#include <GxEPD2_BW.h>
#include <Fonts/FreeSans9pt7b.h>
const GFXfont *font = &FreeSans9pt7b;
#include "GxEPD2_display_selection.h"

#include "alignment.h"
#include "elements.h"

uint8_t channel_selection = 0;

void logTime(const char* label, uint32_t start_time)
{
  if (DEBUG)
  {
    Serial.print("partialRefresh: ");
    Serial.print(label);
    Serial.print(" = ");
    Serial.print(millis() - start_time);
    Serial.println(" ms");
  }
}

Container channels = Container(Container::HORIZONTAL);
Container infoBox = Container(Container::HORIZONTAL);

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

  display.setPartialWindow(channels.x, channels.y, channels.w, channels.h);
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    channels.draw(display);
  } while (display.nextPage());
}

void infoRefresh()
{
  infoBox.x = channels.children[0]->x;
  infoBox.y = infoBox.x;
  Container* p3 = static_cast<Container*>(channels.children[2]);
  infoBox.w = p3->x + p3->w - infoBox.x;
  infoBox.h = p3->y - infoBox.y - 4;
  infoBox.border = true;
  infoBox.borderRadius = 20;
  infoBox.setMargin(8, 8);

  TextElement* infoText = new TextElement("Info", TOP_CENTER);
  infoBox.addChild(infoText);
  infoBox.arrange();
  display.setPartialWindow(infoBox.x, infoBox.y, infoBox.w, infoBox.h);
  display.firstPage();
  do {
    infoBox.draw(display);
  } while (display.nextPage());
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
  channelRefresh();
  infoRefresh();
  do
  {
    channelRefresh();
    channel_selection = channel_selection % 5 + 1;
  } while (channel_selection <= 5);

  display.hibernate();
}

void loop() {};
