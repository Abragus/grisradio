#include <Arduino.h>

#define DEBUG true

#include "gui.h"
#include "radio.h"

Radio radio = Radio();

void setup() {

  if (DEBUG) {
    Serial.begin(115200);
    while (!Serial);
    Serial.println("DEBUG enabled");
  }
  /*
  GUI gui = GUI();
  gui.begin();
  
  gui.setChannel(1);

  gui.setFrequency(92.8);
  gui.setStationName("Radio Trelleborg");
  gui.setVolume(65);
  gui.draw(); */

  radio.setup(7);
  radio.setFrequency(10700);
}

void loop() {
  radio.print();
}

