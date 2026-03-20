// Display Library example for SPI e-paper panels from Dalian Good Display and boards from Waveshare.
// Requires HW SPI and Adafruit_GFX. Caution: the e-paper panels require 3.3V supply AND data lines!
//
// Display Library based on Demo Example from Good Display: https://www.good-display.com/companyfile/32/
//
// Author: Jean-Marc Zingg
// Simplified for ESP32 + WaveShare 2.9" B/W display
//
// Library: https://github.com/ZinggJM/GxEPD2

// Supporting Arduino Forum Topics:
// Good Display ePaper for Arduino: https://forum.arduino.cc/t/good-display-epaper-for-arduino/419657
// Waveshare e-paper displays with SPI: https://forum.arduino.cc/t/waveshare-e-paper-displays-with-spi/467865

// SIMPLIFIED FOR ESP32 ONLY
// Display: WaveShare 2.9" B/W (296x128 with local refresh)
// pin mapping: CS=GPIO5, DC=GPIO17, RST=GPIO16, BUSY=GPIO4

// select the display class (only one), matching the kind of display panel
#define GxEPD2_DISPLAY_CLASS GxEPD2_BW
//#define GxEPD2_DISPLAY_CLASS GxEPD2_3C
//#define GxEPD2_DISPLAY_CLASS GxEPD2_4C
//#define GxEPD2_DISPLAY_CLASS GxEPD2_7C

// select the display driver class (only one) for your  panel
// select the display driver class (only one) for your panel
#define GxEPD2_DRIVER_CLASS GxEPD2_290_T94_V2 // GDEM029T94 128x296, SSD1680, Waveshare 2.9" V2 variant

// Helper macros
#define GxEPD2_BW_IS_GxEPD2_BW true
#define GxEPD2_3C_IS_GxEPD2_3C true
#define GxEPD2_4C_IS_GxEPD2_4C true
#define GxEPD2_7C_IS_GxEPD2_7C true
#define GxEPD2_1248_IS_GxEPD2_1248 true
#define GxEPD2_1248c_IS_GxEPD2_1248c true
#define IS_GxEPD(c, x) (c##x)
#define IS_GxEPD2_BW(x) IS_GxEPD(GxEPD2_BW_IS_, x)
#define IS_GxEPD2_3C(x) IS_GxEPD(GxEPD2_3C_IS_, x)
#define IS_GxEPD2_4C(x) IS_GxEPD(GxEPD2_4C_IS_, x)
#define IS_GxEPD2_7C(x) IS_GxEPD(GxEPD2_7C_IS_, x)
#define IS_GxEPD2_1248(x) IS_GxEPD(GxEPD2_1248_IS_, x)
#define IS_GxEPD2_1248c(x) IS_GxEPD(GxEPD2_1248c_IS_, x)

#include "GxEPD2_selection_check.h"

// ============================================================
// ESP32 Configuration
// ============================================================
// Pin mapping for generic ESP32:
//   CS   -> GPIO5  (SS)
//   DC   -> GPIO17 (Data/Command)
//   RST  -> GPIO16 (Reset)
//   BUSY -> GPIO4  (Busy signal)
//   SCK  -> GPIO18 (SPI Clock) - Hardware SPI
//   MOSI -> GPIO23 (SPI Data)  - Hardware SPI
//   GND  -> GND
//   3.3V -> 3.3V

#if defined(ARDUINO_ARCH_ESP32)

#define MAX_DISPLAY_BUFFER_SIZE 65536ul

#if IS_GxEPD2_BW(GxEPD2_DISPLAY_CLASS)
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8) ? EPD::HEIGHT : MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8))
#elif IS_GxEPD2_3C(GxEPD2_DISPLAY_CLASS) || IS_GxEPD2_4C(GxEPD2_DISPLAY_CLASS)
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= (MAX_DISPLAY_BUFFER_SIZE / 2) / (EPD::WIDTH / 8) ? EPD::HEIGHT : (MAX_DISPLAY_BUFFER_SIZE / 2) / (EPD::WIDTH / 8))
#elif IS_GxEPD2_7C(GxEPD2_DISPLAY_CLASS)
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= (MAX_DISPLAY_BUFFER_SIZE) / (EPD::WIDTH / 2) ? EPD::HEIGHT : (MAX_DISPLAY_BUFFER_SIZE) / (EPD::WIDTH / 2))
#endif

// ESP32 display instantiation with standard pins
// adjust CS, DC, RST, BUSY pins to match your wiring
GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, MAX_HEIGHT(GxEPD2_DRIVER_CLASS)> display(
  GxEPD2_DRIVER_CLASS(/*CS=*/5, /*DC=*/17, /*RST=*/16, /*BUSY=*/4)
);

#undef MAX_DISPLAY_BUFFER_SIZE
#undef MAX_HEIGHT

#else
#error "This sketch is configured for ESP32 only. Please select an ESP32 board in the Arduino IDE."
#endif
