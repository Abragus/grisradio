#ifndef ALIGNMENT_H
#define ALIGNMENT_H

#include <Arduino.h>

struct Point {
  int16_t x, y;
  Point(int16_t x = 0, int16_t y = 0) : x(x), y(y) {}
};

struct Size {
  uint16_t w, h;
  Size(uint16_t w = 0, uint16_t h = 0) : w(w), h(h) {}
};

struct Frame {
    Point origin;
    Size size;
    int16_t &x, &y;
    uint16_t &w, &h;

    // Standard constructor
    Frame(int16_t x, int16_t y, uint16_t w, uint16_t h) 
        : origin(x, y), size(w, h), x(origin.x), y(origin.y), w(size.w), h(size.h) {}

    // Point/Size constructor
    Frame(const Point& origin, const Size& size) 
        : origin(origin), size(size), x(this->origin.x), y(this->origin.y), w(this->size.w), h(this->size.h) {}
        
    // Copy constructor to handle reference reassignment
    Frame(const Frame& other) 
        : origin(other.origin), size(other.size), x(origin.x), y(origin.y), w(size.w), h(size.h) {}
};

struct TextBoxSize {
    Point offset;
    Size size;
    int16_t &tbx, &tby;
    uint16_t &tbw, &tbh;

    TextBoxSize(int16_t x, int16_t y, uint16_t w, uint16_t h) 
        : offset(x, y), size(w, h), tbx(offset.x), tby(offset.y), tbw(size.w), tbh(size.h) {}

    TextBoxSize(const Point& offset, const Size& size) 
        : offset(offset), size(size), tbx(this->offset.x), tby(this->offset.y), tbw(this->size.w), tbh(this->size.h) {}
        
    TextBoxSize(const TextBoxSize& other) 
        : offset(other.offset), size(other.size), tbx(offset.x), tby(offset.y), tbw(size.w), tbh(size.h) {}
};

enum Alignment {
  TOP_LEFT, TOP_CENTER, TOP_RIGHT,
  MIDDLE_LEFT, MIDDLE_CENTER, MIDDLE_RIGHT,
  BOTTOM_LEFT, BOTTOM_CENTER, BOTTOM_RIGHT
};

Point anchorElement(Alignment alignment, const Point& anchor, const Size& size);
Point anchorText(Alignment alignment, const Point& anchor, const TextBoxSize& textBoxSize);
Point getElementAnchor(Alignment alignment, const Frame& frame);
Point getTextAnchor(Alignment alignment, const Point& cursor, const TextBoxSize& textBoxSize);
Point alignInsideBox(Alignment alignment, const Frame& outer, const Size& inner, int16_t margin);
Point alignTextInsideBox(Alignment alignment, const Frame& outer, const TextBoxSize& textBoxSize, int16_t margin);

#endif