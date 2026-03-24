#include "gui.h"

GUI::GUI() {
  display.init(115200, true, 2, false);
  display.setRotation(3);
  display.setFont(font);
  display.setTextColor(GxEPD_BLACK);
}

GUI::~GUI() {
  delete root;
}

void GUI::begin() {
  buildLayout();
  applyChannelSelection();
  applyInfo();
  applyVolume();
  draw();
}

void GUI::draw() {
  if (root) {
    root->updateDisplay(display);
    display.hibernate();
  }
}

void GUI::setChannel(uint8_t channel) {
  if (channel < 1) channel = 1;
  if (channel > 4) channel = 4;
  if (channelSelection != channel) {
    channelSelection = channel;
    applyChannelSelection();
    draw();
  }
}

uint8_t GUI::getChannel() const {
  return channelSelection;
}

void GUI::setFrequency(float freq) {
  frequency = freq;
  applyInfo();
  draw();
}

void GUI::setStationName(const String& name) {
  stationName = name;
  applyInfo();
  draw();
}

void GUI::setVolume(uint8_t vol) {
  volume = (vol > 100) ? 100 : vol;
  applyVolume();
  draw();
}

void GUI::buildLayout() {
  delete root;

  root = new Container(Container::VERTICAL);
  root->x = 0;
  root->y = 0;
  root->w = display.width();
  root->h = display.height();
  root->setMargin(6);
  root->childSizes = {6, 1};

  Container* upper = new Container(Container::HORIZONTAL);
  upper->setMargin(0, 0, 6);
  upper->childSizes = {7, 2};

  buildInfo();
  buildVolume();
  buildChannels();

  upper->addChild(infoBox);
  upper->addChild(volumeBox);

  root->addChild(upper);
  root->addChild(channels);
}

void GUI::buildChannels() {
  channels = new Container(Container::HORIZONTAL);
  channels->setMargin(0, 0, 8);

  const String bottomTexts[] = {"P1", "P2", "P3", "P4"};
  const uint8_t boxRadius = 12;

  for (uint8_t i = 0; i < 4; i++) {
    Container* channelBox = new Container();
    channelBox->border = true;
    channelBox->borderRadius = boxRadius;
    channelBox->setMargin(8, 4);

    TextElement* channelText = new TextElement(bottomTexts[i], TOP_CENTER);
    channelBox->addChild(channelText);
    channels->addChild(channelBox);
  }

  channels->padding_bottom = -1 * (boxRadius + 6);
}

void GUI::buildInfo() {
  infoBox = new Container(Container::VERTICAL);
  infoBox->border = true;
  infoBox->borderRadius = 20;
  infoBox->setMargin(12);

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
    uint8_t barMargin = 3;
    Size barSize = {(uint16_t)(w - 2 * barMargin), (uint16_t)((h - 2 * barMargin) * (volume / 100.0f))};
    Point startPoint = alignInsideBox(BOTTOM_CENTER, {x, y, w, h}, barSize, barMargin);
    display.fillRoundRect(startPoint.x, startPoint.y - barMargin, barSize.w, barSize.h, 14 - barMargin, GxEPD_BLACK);
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

void GUI::applyChannelSelection() {
  for (size_t i = 0; i < channels->children.size(); ++i) {
    channels->children[i]->selected = (i + 1 == channelSelection);
  }
}

void GUI::applyInfo() {
  if (frequencyText) frequencyText->text = String(frequency, 1) + " MHz";
  if (stationText) stationText->text = stationName;
}

void GUI::applyVolume() {
  // volumeLevel lambda uses the member volume directly
  (void)volumeLevel;
}
