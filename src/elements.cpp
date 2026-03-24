#include "elements.h"

#ifndef DEBUG
#define DEBUG false
#endif

int invertColor(int color) {
  return (color == GxEPD_BLACK) ? GxEPD_WHITE : GxEPD_BLACK;
}

void Element::setPadding(uint16_t p) {
  padding_top = p;
  padding_right = p;
  padding_bottom = p;
  padding_left = p;
}

void Element::setPadding(uint16_t ph, uint16_t pv) {
  padding_top = pv;
  padding_right = ph;
  padding_bottom = pv;
  padding_left = ph;
}

void Element::setPadding(uint16_t pt, uint16_t pr, uint16_t pb, uint16_t pl) {
  padding_top = pt;
  padding_right = pr;
  padding_bottom = pb;
  padding_left = pl;
}

void Element::updateDisplay(GxEPD2_BW<GxEPD2_290_T94_V2, 296U>& display, int color) {
  display.setPartialWindow(x, y, w, h);
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    this->draw(display, color);
  } while (display.nextPage());
}

void Container::arrange() {
  if (children.empty()) return;
  int16_t arrange_margin = (orientation == HORIZONTAL) ? margin_h : margin_v;
  int16_t total_space = ((orientation == HORIZONTAL) ? w : h) - 2 * arrange_margin - margin_inner * (children.size() - 1);
  float total_arrangement_parts = std::accumulate(childSizes.begin(), childSizes.end(), 0.0);
  float current_pos = (orientation == HORIZONTAL) ? x + margin_h : y + margin_v;

  uint8_t i = 0;
  for (auto child : children) {
    uint16_t element_size = total_space * (childSizes.empty() ? 1.0 / children.size() : (childSizes[i] / total_arrangement_parts));

    if (orientation == HORIZONTAL) {
      child->x = current_pos + child->padding_left;
      child->y = y + margin_v + child->padding_top;
      child->w = element_size - child->padding_left - child->padding_right;
      child->h = h - 2 * margin_v - child->padding_top - child->padding_bottom;
    } else {
      child->x = x + margin_h + child->padding_left;
      child->y = current_pos + child->padding_top;
      child->w = w - 2 * margin_h - child->padding_left - child->padding_right;
      child->h = element_size - child->padding_top - child->padding_bottom;
    }
    current_pos += element_size + margin_inner;
    i++;
  }
}

void Container::draw(Adafruit_GFX& display, int color) {
  this->arrange();
  if (border || DEBUG) {
    if (selected) {
      display.fillRoundRect(x, y, w, h, borderRadius, color);
      color = invertColor(color);
    } else {
      display.drawRoundRect(x, y, w, h, borderRadius, color);
    }
  }

  for (auto child : children) {
    child->draw(display, color);
  }
}

void Container::setMargin(uint16_t m) {
  margin = m;
  margin_h = m;
  margin_v = m;
  margin_inner = m;
}

void Container::setMargin(uint16_t mh, uint16_t mv) {
  margin_h = mh;
  margin_v = mv;
  margin_inner = (orientation == HORIZONTAL) ? margin_h : margin_v;
}

void Container::setMargin(uint16_t mh, uint16_t mv, uint16_t mi) {
  margin_h = mh;
  margin_v = mv;
  margin_inner = mi;
}

void TextElement::draw(Adafruit_GFX& display, int color) {
  int16_t tbx, tby; uint16_t tbw, tbh;
  display.getTextBounds(text.c_str(), 0, 0, &tbx, &tby, &tbw, &tbh);
  Point cursor = alignTextInsideBox(textAlignment, Frame(x, y, w, h), TextBoxSize(tbx, tby, tbw, tbh), 0);
  display.setCursor(cursor.x, cursor.y);
  display.setTextColor(color);
  if (DEBUG) {
    display.drawRect(x, y, w, h, GxEPD_BLACK);
    display.drawRect(cursor.x + tbx, cursor.y + tby, tbw, tbh, GxEPD_BLACK);
  }
  display.print(text);
}

void ShapeElement::draw(Adafruit_GFX& display, int color) {
    if (DEBUG) {
        display.drawRect(x, y, w, h, GxEPD_BLACK);
    }

    if (drawFunc) drawFunc(display, x, y, w, h);
}