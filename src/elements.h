#ifndef ELEMENTS_H
#define ELEMENTS_H

#include <vector>
#include <numeric>
#include <Adafruit_GFX.h>
#include <GxEPD2.h>
#include "GxEPD2_BW.h"
#include "alignment.h"

class Element {
public:
  int16_t x, y, w, h;
  virtual void draw(Adafruit_GFX& display, int color = GxEPD_BLACK) = 0;
  virtual ~Element() {}
  bool selected = false;

  void updateDisplay(GxEPD2_BW<GxEPD2_290_T94_V2, 296U>& display, int color = GxEPD_BLACK);
};

class Container : public Element {
public:
  uint16_t margin = 0, margin_h = 0, margin_v = 0, margin_inner = 0;
  std::vector<Element*> children;
  std::vector<float> childSizes;
  enum Orientation { HORIZONTAL, VERTICAL };
  Orientation orientation;
  bool border = false;
  int16_t borderRadius = 0;

  Container(Orientation orient = HORIZONTAL) : orientation(orient) {}

  ~Container() {
    for (auto child : children) {
        delete child;
    }
  }

  void addChild(Element* child) {
    children.push_back(child);
  }

  void arrange();
  void draw(Adafruit_GFX& display, int color = GxEPD_BLACK) override;

  void setMargin(uint16_t m);

  void setMargin(uint16_t mh, uint16_t mv);

  void setMargin(uint16_t mh, uint16_t mv, uint16_t mi);
};

class TextElement : public Element {
public:
  String text;
  Alignment textAlignment;

  TextElement(const String& t, Alignment align = TOP_LEFT) : text(t), textAlignment(align) {}

  void draw(Adafruit_GFX& display, int color = GxEPD_BLACK) override;
};

using DrawCallback = std::function<void(Adafruit_GFX&, int16_t x, int16_t y, uint16_t w, uint16_t h)>;

class ShapeElement : public Element {
public:
  DrawCallback drawFunc = nullptr;

  void draw(Adafruit_GFX& display, int color = GxEPD_BLACK) override;
};

#endif