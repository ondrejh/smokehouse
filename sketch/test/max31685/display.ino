#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void display_init() {
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  // show initial display buffer contents on the screen
  display.display();
}

void display_temp(uint8_t s) {
  /*Serial.print(s);
  Serial.print(' ');
  Serial.println(temp[s]);*/
  display.clearDisplay();
  display.setTextSize(2);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  display.setCursor(0, 0);
  display.print(s + 1);
  display.setTextSize(3);      // Normal 1:1 pixel scale
  
  char buff[64];
  int blen;
  int t = temp[s] / 10;
  int dt;
  if (temp[s] >= 0)
    dt = temp[s] - t * 10;
  else
    dt = -temp[s] + t * 10;
  if ((t >= 100) || (t <= -10))
    sprintf(buff, "%d C", t);
  else
    sprintf(buff, "%d.%01d C", t, dt);
  blen = strlen(buff);
  int xs = 70 - blen*9;
  display.setCursor(xs, 10);     // Start at top-left corner
  display.print(buff);

  display.setTextSize(2);
  display.setCursor(xs + blen*18 - 32, 5);
  display.print("o");

  display.display();
}
