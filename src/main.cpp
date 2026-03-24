#include <Arduino.h>

#define DEBUG false

#include "gui.h"

void setup() {

  if (DEBUG) {
    Serial.begin(115200);
    while (!Serial);
    Serial.println("DEBUG enabled");
  }

  GUI gui = GUI();
  gui.begin();
  
  gui.setChannel(1);

  gui.setFrequency(92.8);
  gui.setStationName("Radio Trelleborg");
  gui.setVolume(65);
  gui.draw();
}

void loop() {}

