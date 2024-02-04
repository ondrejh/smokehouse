#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "transfer.h"

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
  if (valid[s]) {
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
  }
  else {
    sprintf(buff, "--- C");
  }
  blen = strlen(buff);
  int xs = 70 - blen*9;
  display.setCursor(xs, 10);     // Start at top-left corner
  display.print(buff);

  display.setTextSize(2);
  display.setCursor(xs + blen*18 - 32, 5);
  display.print("o");

  display.display();
}

void display_wifi(char *ssid, IPAddress ip) {
  display.clearDisplay();
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  
  char buff[32];
  int blen;
  //IPAddress ip = WiFi.localIP();
  sprintf(buff, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
  blen = strlen(buff);
  display.setCursor(64 - blen*3, 20);     // Start at top-left corner
  display.print(buff);

  sprintf(buff, "%s", ssid);
  blen = strlen(buff);
  display.setCursor(64 - blen*3, 4);     // Start at top-left corner
  display.print(buff);  

  display.display();
}

void display_single_line(int siz, char *str) {
  display.clearDisplay();
  display.setTextSize(siz);
  display.setTextColor(SSD1306_WHITE);
  display.cp437(true);

  int blen = strlen(str);
  display.setCursor(64 - blen * 3 * siz, 16 - 4 * siz);
  display.print(str);

  display.display();
}

void display_connecting() {
  char buff[32];
  sprintf(buff, "Connecting");
  display_single_line(2, buff);
}

void display_apmode() {
  char buff[32];
  sprintf(buff, "AP mode");
  display_single_line(2, buff);
}

void display_transfer() {
	int x, y;
	for (x=0; x<16; x++) {
		for (y=0; y<24; y++) {
			int B = (y / 8) + 3 * x;
			int b = 7 - (y % 8);
			if (B >= 24) {
				B = 47 - B;
				b = 7 - b;
			}
      display.drawPixel(127 - y, x, ((transf_icon[B] & (1<<b)) != 0) ? 1 : 0);
		}
	}
  display.display();
}