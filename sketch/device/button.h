#ifndef __BUTTON_H__
#define __BUTTON_H__

#include <stdint.h>
#include <Arduino.h>

class Button {
  public:
    Button(const int p) {
      pin = p;
      pinMode(pin, INPUT_PULLUP);
    }
    bool press;
    bool release;
    bool isDown(void);
    void poll(void);
    void poll(uint32_t now);
  private:
    int pin;
    uint32_t t;
    uint8_t mask;
};

#endif
