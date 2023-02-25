#include <Arduino.h>

#include <Adafruit_NeoPixel.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "wifi_password.h"

#ifndef WSSID
// actual wifi credentials should be stored in wifi_password.h (not the part of the project)
#define WSSID "wifi_ssid"
#define WPWD "wifi_password"
#endif

ESP8266WiFiMulti WiFiMulti;

#define LED_PIN D7
#define NUMPIXELS 4

#define TEMP_MIN 55.0
#define TEMP_OPT_MIN 60.0
#define TEMP_OPT_MAX 65.0
#define TEMP_MAX 70.0

Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

// display
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  // backlight initialization
  pixels.begin();
  for (int i=0; i<NUMPIXELS; i++)
    pixels.setPixelColor(i++, pixels.Color(0, 0, 0));
  
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))// Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));

  // show initial display buffer contents on the screen
  display.display();

  // wifi connect
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(WSSID, WPWD);
}

// the loop function runs over and over again forever
void loop() {
  static uint8_t i;
  static uint8_t r=0, g=0, b=0;

  pixels.setPixelColor(i++, pixels.Color(r, g, b));
  pixels.show();
  i %= NUMPIXELS;

  Serial.print(i);
  Serial.print(": ");
  Serial.print(r);
  Serial.print(",");
  Serial.print(g);
  Serial.print(",");
  Serial.println(b);
  delay(1000);

  static uint32_t wget_timer = 0;
  if ((millis() - wget_timer) > 10000) {
    if (WiFiMulti.run() == WL_CONNECTED) {
      digitalWrite(LED_BUILTIN, LOW);
      WiFiClient client;
      HTTPClient http;
  
      if (http.begin(client, "http://www.kegator.cz/tlast.php")) {
        int httpCode = http.GET();
        if (httpCode > 0) {
          if ((httpCode == HTTP_CODE_OK) || (httpCode == HTTP_CODE_MOVED_PERMANENTLY)) {
            String payload = http.getString();

            // get data from payload
            String s_temp = strip_string(find_key_simple(payload, "temp"));
            String s_time = strip_string(find_key_simple(payload, "tstamp"));
            String s_tout = strip_string(find_key_simple(payload, "tout"));
            float temp = s_temp.toFloat();
            int tout = s_tout.toInt();
            int bb, e;
            e = s_time.lastIndexOf(":");
            bb = s_time.lastIndexOf(" ") + 1;
            s_time = s_time.substring(bb, e);

            // display data
            if (tout < 180) {
              Serial.print(s_time);
              Serial.print(" ");
              Serial.print(temp);
              Serial.print(" (");
              Serial.print(tout);
              Serial.println(")");
              /*static bool up = true;
              static float temp = 68.9;
              temp += up ? 1.1 : -1.1;
              if (temp > 120.0) up = false;
              if (temp < -20.0) up = true;
              Serial.println(temp);*/
              display_temp(temp);
              if (temp < TEMP_MIN) {
                r = 0; g = 0; b = 255;
              }
              else if (temp < TEMP_OPT_MIN) {
                g = uint8_t(float(255) * (TEMP_MIN - temp) / (TEMP_MIN - TEMP_OPT_MIN));
                b = 255 - g;
                r = 0;
              }
              else if (temp < TEMP_OPT_MAX) {
                r = 0;
                g = 255;
                b = 0;
              }
              else if (temp < TEMP_MAX) {
                r = uint8_t(float(255) * (TEMP_OPT_MAX - temp) / (TEMP_OPT_MAX - TEMP_MAX));
                g = 255 - r;
                b = 0;
              }
              else /*if (temp > TEMP_MAX)*/ {
                r = 255; g = 0; b = 0;
              }
            }
            else {
              r = 100; g = 100; b = 100;
              display_no_signal();
            }
          }
        }
      }
      digitalWrite(LED_BUILTIN, HIGH);
    }
    else {
      // no signal
      Serial.println("Not connected");
      display_no_signal();
      r = 0; g = 0; b = 0;
    }
    wget_timer = millis();
  }
}
