#include "valium_graphics.h"

struct ValiumGraphics::impl {

};

ValiumGraphics::ValiumGraphics() {
  _impl = new impl();
}

ValiumGraphics::~ValiumGraphics() {
  delete _impl;
}
