#include "gui.h"

GUI::GUI() {
  display.init(115200, true, 2, false, SPI, SPISettings(20000000, MSBFIRST, SPI_MODE0));
  display.setRotation(3);
  display.setFont(font);
  display.setTextColor(GxEPD_BLACK);
}

GUI::~GUI() {
  delete root;
}

void GUI::begin() {
  buildLayout();
  applyInfo();
  applyVolume();
  xTaskCreate(displayWorker, "displayWorker", 2048, (void*)this, 1, &displayWorkerHandle);
}

void GUI::draw() {
  needsRedraw = true;
  if (displayWorkerHandle != NULL) {
      xTaskNotifyGive(displayWorkerHandle);
  }
}

void GUI::displayWorker(void* param) {
    GUI* gui = (GUI*)param;

    while (true) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        while (gui->needsRedraw) {
            gui->needsRedraw = false;
            gui->root->updateDisplay(gui->display);
        }
        gui->display.hibernate();
    }
}

void GUI::setFrequency(float freq) {
  frequency = freq;
  updateActivePreset();
  applyInfo();
  draw();
}

void GUI::changeFrequency(float delta) {
  setFrequency(frequency + delta);
}

float GUI::getFrequency() const {
  return frequency;
}

bool GUI::setPresetFrequency(uint8_t preset, float freq) {
  if (preset < 1 || preset > 4) return false;
  
  presetFrequencies[preset - 1] = freq;
  updateActivePreset();
  
  return true;
}

bool GUI::setPresetFrequency(uint8_t preset) {
  bool success = setPresetFrequency(preset, frequency);
  Serial.printf("Set preset %d frequency to %.1f MHz\n", preset, frequency);
  activePreset = 0;
  activatePreset(preset);
  return success;
}

float GUI::getPresetFrequency(uint8_t preset) const {
  if (preset < 1 || preset > 4) return -1;
  return presetFrequencies[preset - 1];
}

float GUI::activatePreset(uint8_t preset) {
  if (preset < 1 || preset > 4) return -1;

  if (activePreset != preset) {
    
    activePreset = preset;
    setFrequency(getPresetFrequency(preset));
  }

  return getPresetFrequency(preset);
}

uint8_t GUI::getActivePreset() const {
  return activePreset;
}

void GUI::updateActivePreset() {
  if (frequency == getPresetFrequency(activePreset));
  else if (frequency == getPresetFrequency(1)) activePreset = 1;
  else if (frequency == getPresetFrequency(2)) activePreset = 2;
  else if (frequency == getPresetFrequency(3)) activePreset = 3;
  else if (frequency == getPresetFrequency(4)) activePreset = 4;
  else activePreset = 0;

  for (size_t i = 0; i < presetsBox->children.size(); ++i) {
    presetsBox->children[i]->selected = (i + 1 == activePreset);
  }
}

void GUI::setStationName(const String& name) {
  stationName = name;
  applyInfo();
  draw();
}

String GUI::getStationName() const {
  return stationName;
}

void GUI::setVolume(int8_t vol) {
  volume = (vol > maxVolume) ? maxVolume : (vol < 0) ? 0 : vol;
  applyVolume();
  draw();
}

void GUI::changeVolume(int8_t delta) {
  if (volumeMuted) {
    volumeMuted = false;
    setVolume(delta);
    return;
  }
  setVolume(volume + delta);
}

void GUI::toggleVolumeMute() {
  volumeMuted = !volumeMuted;
  applyVolume();
  draw();
}

uint8_t GUI::getVolume() const {
  return (volumeMuted ? 0 : volume);
}

void GUI::buildLayout() {
  delete root;

  root = new Container(Container::VERTICAL);
  root->x = 0;
  root->y = 0;
  root->w = display.width();
  root->h = display.height();
  root->setMargin(6);
  root->childSizes = {7, 1};

  Container* upper = new Container(Container::HORIZONTAL);
  upper->setMargin(0, 0, 6);
  upper->childSizes = {7.1, 2};

  buildInfo();
  buildVolume();
  buildPresets();

  upper->addChild(infoBox);
  upper->addChild(volumeBox);

  root->addChild(upper);
  root->addChild(presetsBox);
}

void GUI::buildPresets() {
  presetsBox = new Container(Container::HORIZONTAL);
  presetsBox->setMargin(0, 0, 8);

  const String bottomTexts[] = {"P1", "P2", "P3", "P4"};
  const uint8_t boxRadius = 12;

  for (uint8_t i = 0; i < 4; i++) {
    Container* presetBox = new Container();
    presetBox->border = true;
    presetBox->borderRadius = boxRadius;
    presetBox->setMargin(8, 4);

    TextElement* presetText = new TextElement(bottomTexts[i], TOP_CENTER);
    presetBox->addChild(presetText);
    presetsBox->addChild(presetBox);
  }

  presetsBox->padding_bottom = -1 * (boxRadius + 6);
}

void GUI::buildInfo() {
  infoBox = new Container(Container::VERTICAL);
  infoBox->border = true;
  infoBox->borderRadius = 20;
  infoBox->setMargin(12);
  infoBox->margin_inner = 4;
  infoBox->childSizes = {1, 3};

  frequencyText = new TextElement(String(frequency, 1) + " MHz", TOP_LEFT);
  stationText = new TextElement(stationName, TOP_LEFT);

  infoBox->addChild(frequencyText);
  infoBox->addChild(stationText);
}

void GUI::buildVolume() {
  Container* volumeBox = new Container(Container::HORIZONTAL);
  volumeBox->border = true;
  volumeBox->borderRadius = 20;
  volumeBox->setMargin(6);
  volumeBox->padding_right = -1 * (volumeBox->borderRadius + 6);
  volumeBox->childSizes = {1, 2};

  Container* volumeBar = new Container();
  volumeBar->border = true;
  volumeBar->borderRadius = 14;

  volumeLevel = new ShapeElement();
  volumeLevel->drawFunc = [this](Adafruit_GFX& display, int16_t x, int16_t y, uint16_t w, uint16_t h) {
    if (getVolume() == 0) return;

    uint8_t barMargin = 3, fillet = 14 - barMargin;
    Size barSize = {(uint16_t)(w - 2 * barMargin), (uint16_t)((h - 2 * barMargin) * (volume / (float)maxVolume))};
    uint8_t cutoffHeight = 0, minOffset = 6;
    
    // Detect low volume, draw white box over top part of volume bar to retain correct fillet shape
    if (barSize.h <= 2 * fillet - minOffset) {
      cutoffHeight = 2 * fillet + 1 - minOffset - barSize.h;
      barSize.h = 2 * fillet + 1 - minOffset;
    }
    
    Point startPoint = alignInsideBox(BOTTOM_CENTER, {x, y, w, h}, barSize, barMargin);
    
    display.fillRoundRect(startPoint.x, startPoint.y - barMargin, barSize.w, barSize.h, fillet, GxEPD_BLACK);

    if (cutoffHeight > 0) {
      display.fillRect(startPoint.x, startPoint.y - barMargin, barSize.w, cutoffHeight, GxEPD_WHITE);
    }
  };

  volumeBar->addChild(volumeLevel);
  volumeBox->addChild(volumeBar);

  Container* volumeIcons = new Container(Container::VERTICAL);
  volumeIcons->setMargin(0, 0, volumeBox->margin_inner);

  Container* plusButton = new Container();
  plusButton->border = true;
  plusButton->borderRadius = 14;
  plusButton->setMargin(8);
  ShapeElement* plusSymbol = new ShapeElement();
  plusSymbol->drawFunc = [](Adafruit_GFX& display, int16_t x, int16_t y, uint16_t w, uint16_t h) {
    const int16_t lineLength = min((int16_t)w, (int16_t)h);
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
    const int16_t lineLength = min((int16_t)w, (int16_t)h);
    const int8_t lineWidth = 3;
    Frame hLine = {alignInsideBox(MIDDLE_LEFT, {x, y, w, h}, Size(lineLength, lineWidth), 0), Size(lineLength, lineWidth)};
    display.fillRoundRect(hLine.x, hLine.y, lineLength, lineWidth, lineWidth, GxEPD_BLACK);
  };
  minusButton->addChild(minusSymbol);
  volumeIcons->addChild(minusButton);

  volumeBox->addChild(volumeIcons);

  // keep pointer to volume box so buildLayout can place it in the upper section
  this->volumeBox = volumeBox;
}

void GUI::applyInfo() {
  if (frequencyText) frequencyText->text = String(frequency, 1) + " MHz";
  if (stationText) stationText->text = stationName;
}

void GUI::applyVolume() {
  // volumeLevel lambda uses the member volume directly
  (void)volumeLevel;
}
