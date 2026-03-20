#include <Arduino.h>

const bool DEBUG = false;

// Set display class
#include <GxEPD2_BW.h>
#include <Fonts/FreeSans9pt7b.h>
const GFXfont *font = &FreeSans9pt7b;
#include "GxEPD2_display_selection.h"

const char HelloWorld[] = "Hello World!";
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

void partialRefresh()
{
  uint32_t t_start = 0;
  if (DEBUG)
  {
    t_start = millis();
    Serial.println("partialRefresh: start");
  }

  const String bottom_texts[] = {"P1", "P2", "P3", "P4"};
  const uint8_t num_items = sizeof(bottom_texts) / sizeof(bottom_texts[0]);
  const int16_t gap = 10, margin = 8, box_margin = 4, box_margin_h = 16, corner_radius = 12;
  int16_t texts_x[num_items], y=display.height(), box_w = 0, box_h = 0;
  
  for (uint8_t i = 0; i < num_items; i++)
  {
    int16_t tbx, tby; uint16_t tbw, tbh;
    display.getTextBounds(bottom_texts[i].c_str(), 0, 0, &tbx, &tby, &tbw, &tbh);
    box_w = max(box_w, int16_t(tbw + 2 * box_margin_h));
    box_h = max(box_h, int16_t(tbh + 2 * box_margin));
  }

  logTime("text bound measure", t_start);

  const int16_t dist = (display.width() - 2 * gap - box_w) / (num_items - 1);

  for (uint8_t i = 0; i < num_items; i++)
  {
    int16_t tbx, tby; uint16_t tbw, tbh;
    display.getTextBounds(bottom_texts[i].c_str(), 0, 0, &tbx, &tby, &tbw, &tbh);
  
    texts_x[i] = gap + box_w / 2 + i * dist - tbw / 2 - tbx;
    
    y = min(y, int16_t(display.height() - margin/2)); 
  }

  logTime("coordinate calc", t_start);

  int16_t partialWindow[4] = {int16_t(gap - margin), int16_t(y - box_h + box_margin - margin), int16_t((display.width() - 2 * gap + 2 * margin)), int16_t(box_h + 2 * margin - 1)};
  display.setPartialWindow(partialWindow[0], partialWindow[1], partialWindow[2], partialWindow[3]);
  display.firstPage();
  logTime("setPartialWindow and firstPage", t_start);
  do {
    logTime("start page loop iteration", t_start);
    display.fillScreen(GxEPD_WHITE);
    logTime("fillScreen", t_start);
    for (uint8_t i = 0; i < num_items; i++)
    {
      if (i + 1 == channel_selection)
      {
        display.fillRoundRect(texts_x[i] - box_margin_h, y - box_h + box_margin, box_w, box_h + 2*corner_radius, corner_radius, GxEPD_BLACK);
        display.setTextColor(GxEPD_WHITE);
      } else
      {
        display.drawRoundRect(texts_x[i] - box_margin_h, y - box_h + box_margin, box_w, box_h + 2*corner_radius, corner_radius, GxEPD_BLACK);
        display.setTextColor(GxEPD_BLACK);
      }
      display.setCursor(texts_x[i], y);
      display.print(bottom_texts[i]);
      
      
      if (DEBUG)
      {
        // Draw rect around text with margins removed for debugging
        display.drawRect(texts_x[i], y - box_h + 2 * box_margin, box_w - 2 * box_margin_h, box_h - 2 * box_margin, GxEPD_BLACK);
        // Draw border around the entire partialWindow for debugging
        display.drawRect(partialWindow[0], partialWindow[1], partialWindow[2], partialWindow[3], GxEPD_BLACK);
      }
    }
    logTime("draw elements", t_start);
  } while (display.nextPage());
  logTime("loop+draw total", t_start);
}

void helloWorld()
{
  int16_t tbx, tby; uint16_t tbw, tbh;
  display.getTextBounds(HelloWorld, 0, 0, &tbx, &tby, &tbw, &tbh);
  // center the bounding box by transposition of the origin:
  uint16_t x = ((display.width() - tbw) / 2) - tbx;
  uint16_t y = ((display.height() - tbh) / 2) - tby;
  display.setFullWindow();
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(x, y);
    display.print(HelloWorld);

    if (DEBUG)
    {
      // Draw outline around entire display
      display.drawRect(0, 0, display.width(), display.height(), GxEPD_BLACK);
    }
  }
  while (display.nextPage());
}

void setup()
{
  //display.init(115200); // default 10ms reset pulse, e.g. for bare panels with DESPI-C02
  display.init(115200, true, 2, false);
  display.setRotation(1);
  display.setFont(font);
  display.setTextColor(GxEPD_BLACK);

  if (DEBUG)
  {
    Serial.begin(115200);
    while (!Serial) ;
    Serial.println("DEBUG enabled: serial timing logs on");
  }
  helloWorld();
  do
  {
    // Add C++ timer to check how long the refresh takes
    partialRefresh();
    channel_selection = channel_selection % 5 + 1;
    // delay(3000);
  } while (channel_selection <= 5);

  display.hibernate();
}

void loop() {};
