#include <Adafruit_MAX31865.h>

#include "button.h"

#define TEMP_READ_PERIOD_MS 5000
#define DISPLAY_CYCLE_PERIOD_MS 5000
#define BUTTON_PRESS_CYCLE_PAUSE_MS 15000

const int led = 8;
const int btnPin = 2;

int temp[2];

// Use software SPI: CS, DI, DO, CLK
//Adafruit_MAX31865 thermo = Adafruit_MAX31865(D8, D7, D6, D5); // 10k pulldown on D8
Adafruit_MAX31865 thermo1 = Adafruit_MAX31865(27, 26, 15, 14);
Adafruit_MAX31865 thermo2 = Adafruit_MAX31865(28, 26, 15, 14);

Button btn = Button(btnPin);

void setup(void) {
  Serial.begin(115200);
  delay(300);
  Serial.println();

  pinMode(0, OUTPUT);
  digitalWrite(0, 0); // gnd for button
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);

  thermo1.begin(MAX31865_3WIRE);
  thermo2.begin(MAX31865_3WIRE);

  display_init();
}

void loop(void) {
  uint32_t now = millis();
  
  btn.poll();
  if (btn.press) Serial.println("Pressed");
  if (btn.release) Serial.println("Released");

  static uint32_t tempT = TEMP_READ_PERIOD_MS;
  static int tempCh = 0;
  if ((now - tempT) >= TEMP_READ_PERIOD_MS) { 
    temp[tempCh] = read_temperature(tempCh);
    tempCh ++;
    if (tempCh >= 2) {
      tempT = now;
      tempCh = 0;
    }
  }

  static uint32_t dispT = 0;
  static int disp = 0;
  static uint32_t period = DISPLAY_CYCLE_PERIOD_MS;
  if ( ((now - dispT) >= period) || btn.press ) {
    dispT = now;
    period = btn.press ? BUTTON_PRESS_CYCLE_PAUSE_MS : DISPLAY_CYCLE_PERIOD_MS;

    switch (disp) {
      case 0:
        display_temp(0);
        break;
      case 1:
        display_temp(1);
        break;
      default:
        break;
    }
    
    disp ++;
    disp %= 2;

    digitalWrite(led, !digitalRead(led));
  }
}
