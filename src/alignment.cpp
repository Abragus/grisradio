#include "alignment.h"

Point anchorElement(Alignment alignment, const Point& anchor, const Size& size) {
  int16_t x = anchor.x;
  int16_t y = anchor.y;
  uint16_t w = size.w;
  uint16_t h = size.h;

  switch (alignment) {
    case TOP_LEFT:      return {x, y};
    case TOP_CENTER:    return {(int16_t)(x - w / 2), y};
    case TOP_RIGHT:     return {(int16_t)(x - w), y};
    case MIDDLE_LEFT:   return {x, (int16_t)(y - h / 2)};
    case MIDDLE_CENTER: return {(int16_t)(x - w / 2), (int16_t)(y - h / 2)};
    case MIDDLE_RIGHT:  return {(int16_t)(x - w), (int16_t)(y - h / 2)};
    case BOTTOM_LEFT:   return {x, (int16_t)(y - h)};
    case BOTTOM_CENTER: return {(int16_t)(x - w / 2), (int16_t)(y - h)};
    case BOTTOM_RIGHT:  return {(int16_t)(x - w), (int16_t)(y - h)};
    default:            return {x, y};
  }
}

Point anchorText(Alignment alignment, const Point& anchor, const TextBoxSize& tbs) {
  Point aligned = anchorElement(alignment, anchor, tbs.size);
  return {
    (int16_t)(aligned.x - tbs.tbx), 
    (int16_t)(aligned.y - tbs.tby)
  };
}

Point getElementAnchor(Alignment alignment, const Frame& frame) {
  int16_t ref_x = frame.x;
  int16_t ref_y = frame.y;
  uint16_t w = frame.w;
  uint16_t h = frame.h;

  switch (alignment) {
    case TOP_LEFT:      return {ref_x, ref_y};
    case TOP_CENTER:    return {(int16_t)(ref_x + w / 2), ref_y};
    case TOP_RIGHT:     return {(int16_t)(ref_x + w), ref_y};
    case MIDDLE_LEFT:   return {ref_x, (int16_t)(ref_y + h / 2)};
    case MIDDLE_CENTER: return {(int16_t)(ref_x + w / 2), (int16_t)(ref_y + h / 2)};
    case MIDDLE_RIGHT:  return {(int16_t)(ref_x + w), (int16_t)(ref_y + h / 2)};
    case BOTTOM_LEFT:   return {ref_x, (int16_t)(ref_y + h)};
    case BOTTOM_CENTER: return {(int16_t)(ref_x + w / 2), (int16_t)(ref_y + h)};
    case BOTTOM_RIGHT:  return {(int16_t)(ref_x + w), (int16_t)(ref_y + h)};
    default:            return {ref_x, ref_y};
  }
}

Point getTextAnchor(Alignment alignment, const Point& cursor, const TextBoxSize& tbs) {
  Point aligned = getElementAnchor(alignment, Frame(cursor.x + tbs.tbx, cursor.y + tbs.tby, tbs.tbw, tbs.tbh));
  return aligned;
}

Point alignInsideBox(Alignment alignment, const Frame& outer, const Size& inner, int16_t margin) {
  Point anchor = getElementAnchor(alignment, outer);
  return anchorElement(alignment, anchor, inner);
}

Point alignTextInsideBox(Alignment alignment, const Frame& outer, const TextBoxSize& tbs, int16_t margin) {
  Point aligned = alignInsideBox(alignment, outer, Size(tbs.tbw, tbs.tbh), margin);
  return {
    (int16_t)(aligned.x - tbs.tbx), 
    (int16_t)(aligned.y - tbs.tby)
  };
}