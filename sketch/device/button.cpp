#include "button.h"
#ifdef __AVR
#include <avr/pgmspace.h>
#elif defined(ESP8266)
#include <pgmspace.h>
#endif

#include <stdlib.h>

bool Button::isDown(void) {
  return digitalRead(pin) == LOW;
}

void Button::poll(void) {
  uint32_t now = millis();
  Button::poll(now);
}

void Button::poll(uint32_t now) {
  if ((now - t) > 2) {
    t = now;
    bool b = Button::isDown();

    uint8_t lMask = mask;
    mask <<= 1;
    if (b)
      mask |= 1;

    press = ((mask == 0xFF) && (lMask != 0xFF));
    release = ((mask == 0x00) && (lMask != 0x00));
  }
  else {
    press = false;
    release = false;
  }
}
