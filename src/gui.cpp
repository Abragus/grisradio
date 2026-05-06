#include "gui.h"
#include "pinin.h"

GUI::GUI() {
  SPI.begin(DISPLAY_CLK, -1, DISPLAY_DIN, DISPLAY_CS);
  display.init(115200, true, 2, false, SPI, SPISettings(2000000, MSBFIRST, SPI_MODE0));
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

void GUI::setTime(const String& time) {
  this->time = time;
  applyInfo();
  draw();
}

String GUI::getTime() const {
  return time;
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

void GUI::setVolumeMute(bool mute) {
  volumeMuted = mute;
  applyVolume();
  draw();
}

void GUI::toggleVolumeMute() {
  volumeMuted = !volumeMuted;
  applyVolume();
  draw();
}

bool GUI::isVolumeMuted() const {
  return volumeMuted;
}

uint8_t GUI::getVolume() const {
  return (volumeMuted ? 0 : volume);
}

void GUI::setBatteryLevel(float level) {
  batteryLevel = static_cast<uint8_t>((level > 100) ? 100 : (level < 0) ? 0 : level);
  applyInfo();
  draw();
}

uint8_t GUI::getBatteryLevel() const {
  return (batteryLevel == -1) ? 0 : batteryLevel;
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
  upper->childSizes = {10, 1};

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

  const String bottomTexts[] = {"CH1", "CH2", "CH3", "CH4"};
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
  infoBox->setMargin(10);
  infoBox->margin_inner = 8;
  infoBox->childSizes = {1, 4};

  frequencyText = new TextElement(String(frequency, 1) + " MHz", TOP_LEFT);
  stationText = new TextElement(stationName, TOP_LEFT);
  timeText = new TextElement(time, TOP_RIGHT);

  topBar = new Container(Container::HORIZONTAL);
  topBar->margin_inner = 4;
  topBar->addChild(frequencyText);

  batteryIcon = new ShapeElement();
  batteryIcon->drawFunc = [this](Adafruit_GFX& display, int16_t x, int16_t y, uint16_t w, uint16_t h) {
    float w_to_h_ratio = 1.8;
    int16_t minWidth = (int16_t)(h * w_to_h_ratio < w) ? h * w_to_h_ratio : w;
    Size size = {(uint16_t)(minWidth), (uint16_t)(minWidth / w_to_h_ratio)};
    Size capSize = {(uint16_t)(size.w * 0.1), (uint16_t)(size.h * 0.3)};
    uint8_t bodyWidth = size.w - capSize.w;
    uint8_t rounding = static_cast<uint8_t>(size.w * 0.1);
    uint8_t batteryMargin = 2;

    // Make sure cap can be centered on body (both heights even or both odd)
    capSize.h += (1 + capSize.h + size.h) % 2;

    Point corner = alignInsideBox(TOP_RIGHT, {x, y, w, h}, size, 0);

    display.drawRoundRect(corner.x, corner.y, bodyWidth, size.h, rounding + 1, GxEPD_BLACK);

    int8_t capRounding;
    for (uint8_t i = 0; i < capSize.w; i++) {
      capRounding = ((i == capSize.w - 1));
      display.drawLine( corner.x + bodyWidth + i, 
                        corner.y + (size.h - capSize.h) / 2 + capRounding, 
                        corner.x + bodyWidth + i, 
                        corner.y + (size.h + capSize.h) / 2 - capRounding, GxEPD_BLACK);
    }

    display.fillRoundRect(corner.x + batteryMargin, corner.y + batteryMargin, (bodyWidth - 2 * batteryMargin) * batteryLevel / 100, size.h - 2 * batteryMargin, rounding, GxEPD_BLACK);
  };
  
  infoBox->addChild(topBar);
  infoBox->addChild(stationText);
}

void GUI::buildVolume() {
  Container* volumeBox = new Container(Container::VERTICAL);
  volumeBox->border = true;
  volumeBox->borderRadius = 20;
  volumeBox->setMargin(6, 6, 4);
  volumeBox->padding_right = -1 * (volumeBox->borderRadius + volumeBox->margin_h);
  volumeBox->childSizes = {4, 1};

  Container* volumeBar = new Container();
  volumeBar->border = true;
  volumeBar->borderRadius = 14;
  volumeBar->padding_right = volumeBox->borderRadius - volumeBox->margin_h;

  volumeLevel = new ShapeElement();
  volumeLevel->drawFunc = [this](Adafruit_GFX& display, int16_t x, int16_t y, uint16_t w, uint16_t h) {
    if (volume == 0) return;

    uint8_t barMargin = 3, fillet = 14 - barMargin;
    Size barSize = {(uint16_t)(w - 2 * barMargin), (uint16_t)((h - 2 * barMargin) * (volume / (float)maxVolume))};
    uint8_t cutoffHeight = 0, minOffset = 6;
    
    // Detect low volume, draw white box over top part of volume bar to retain correct fillet shape
    if (barSize.h <= 2 * fillet - minOffset) {
      cutoffHeight = 2 * fillet + 1 - minOffset - barSize.h;
      barSize.h = 2 * fillet + 1 - minOffset;
    }
    
    Point startPoint = alignInsideBox(BOTTOM_CENTER, {x, y, w, h}, barSize, barMargin);
    
    if (!isVolumeMuted()) {
      display.fillRoundRect(startPoint.x, startPoint.y - barMargin, barSize.w, barSize.h, fillet, GxEPD_BLACK);
    } else if (volume >= maxVolume / 3) {
      display.drawRoundRect(startPoint.x, startPoint.y - barMargin, barSize.w, barSize.h, fillet, GxEPD_BLACK);
    }

    if (cutoffHeight > 0) {
      display.fillRect(startPoint.x, startPoint.y - barMargin, barSize.w, cutoffHeight, GxEPD_WHITE);
    }
  };

  ShapeElement* volumeIcon = new ShapeElement();
  volumeIcon->padding_right = volumeBar->padding_right;
  volumeIcon->drawFunc = [this](Adafruit_GFX& display, int16_t x, int16_t y, uint16_t w, uint16_t h) {
    float h_to_w_ratio = 0.8;
    int16_t minHeight = (int16_t)(w * h_to_w_ratio < h) ? w * h_to_w_ratio : h;
    minHeight += (minHeight + 1) % 2;
    Size size = {(uint16_t)(minHeight / h_to_w_ratio), (uint16_t)(minHeight)};

    const uint8_t maxWaves = 3;
    uint8_t waveCount = std::ceil((getVolume() / (float)maxVolume) * 3);
    uint8_t soundWaveDistance = size.w * 0.19;
    uint8_t soundWaveCenterOffset = size.h * 0.2;

    uint8_t muteLineWidth = 2;
    int16_t actualWidth;

    if (isVolumeMuted()) {
      actualWidth = 1 + size.h * 0.8;
    } else {
      actualWidth = size.w - (maxWaves - waveCount) * soundWaveDistance; // Simplified sound wave width calculation from drawArc parameters
    }

    Point corner = alignInsideBox(MIDDLE_CENTER, {x, y, w, h}, {static_cast<uint16_t>(actualWidth), static_cast<uint16_t>(size.h)}, 0);

    uint8_t rounding = 1;
    Point center = {static_cast<int16_t>(corner.x + size.w / 2), static_cast<int16_t>(corner.y + size.h / 2)};
    Size rect = {static_cast<uint16_t>(size.w / 3), static_cast<uint16_t>(size.h * 0.5)};
    rect.h += (rect.h + 1) % 2;
    Point rectCorner = alignInsideBox(MIDDLE_LEFT, {corner.x, corner.y, size.w, size.h}, rect, 0);
    Size triangle = {static_cast<uint16_t>(size.w * 0.5), static_cast<uint16_t>(size.h)};
    Point triCorner = {static_cast<int16_t>(corner.x + size.w * 0.05), center.y};

    display.fillRoundRect(rectCorner.x, rectCorner.y, rect.w, rect.h, rounding, GxEPD_BLACK);

    for (int16_t i = 0; i <= (triangle.h) / 2; i++) {
      int8_t triangleRounding = (i == (triangle.h) / 2) ? 0 : -1;
      display.drawLine(triCorner.x + 1 + i, triCorner.y - i + triangleRounding, triCorner.x + 1 + i, triCorner.y + i - triangleRounding, GxEPD_BLACK);
    }

    if (isVolumeMuted()) {
      for (uint8_t i = 0; i <= muteLineWidth; i++) {
        uint16_t color = (i == 0) ? GxEPD_WHITE : GxEPD_BLACK;
        display.drawLine(corner.x - muteLineWidth + i, corner.y + size.h * 0.1, corner.x - muteLineWidth + 1 + i + size.h * 0.8, corner.y + size.h * 0.9, color);
      }
    } else {
      for (int8_t i = 2; i >= maxWaves - waveCount; i--) {
        uint8_t angle = 40 + 1 * i;
        for (int8_t j = 0; j <= (soundWaveDistance >= 6 ? 1 : 0); j++) {
          drawArc(display, center.x - soundWaveCenterOffset, center.y, size.w / 2  + soundWaveCenterOffset - i * soundWaveDistance - j, -1 * angle, angle, GxEPD_BLACK);
        }
      }
    }
  };

  volumeBar->addChild(volumeLevel);
  volumeBox->addChild(volumeBar);
  volumeBox->addChild(volumeIcon);

  this->volumeBox = volumeBox;
}

void GUI::applyInfo() {
  if (frequencyText) frequencyText->text = String(frequency, 1) + " MHz";
  if (stationText) stationText->text = stationName;
  if (timeText) timeText->text = time;

  const uint8_t topBarParts = 5;
  topBar->childSizes = {topBarParts};
  topBar->children = {frequencyText};
  
  // Fix top bar layout based on which elements are defined
  if (batteryLevel != -1) {
    topBar->childSizes.push_back(1);
    topBar->addChild(batteryIcon);
  }

  if (timeText->text.length() > 0) {
    topBar->childSizes.push_back(1);
    topBar->addChild(timeText);
  }

  topBar->childSizes[0] = topBarParts - (topBar->childSizes.size() - 1);
}

void GUI::applyVolume() {
  // volumeLevel lambda uses the member volume directly
  (void)volumeLevel;
}
