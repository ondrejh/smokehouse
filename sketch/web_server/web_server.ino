#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <Adafruit_MAX31865.h>

#include "secret_password.h"
#include "button.h"

#ifndef STASSID
#define STASSID "ssid"
#define STAPSK  "password"
#endif

#define TEMP_READ_PERIOD_MS 10000
#define DISPLAY_CYCLE_PERIOD_MS 5000
#define BUTTON_PRESS_CYCLE_PAUSE_MS 15000

const char *ssid = STASSID;
const char *password = STAPSK;

ESP8266WebServer server(80);

const int led = 13;
const int btnPin = 16;

int temp[2];

String key = SECRET_KEY;
char idstr[16] = "";

// Use software SPI: CS, DI, DO, CLK
//Adafruit_MAX31865 thermo = Adafruit_MAX31865(D8, D7, D6, D5); // 10k pulldown on D8
Adafruit_MAX31865 thermo1 = Adafruit_MAX31865(2, 13, 12, 14);
Adafruit_MAX31865 thermo2 = Adafruit_MAX31865(0, 13, 12, 14);

Button btn = Button(btnPin);

void setup(void) {
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);

  thermo1.begin(MAX31865_3WIRE);
  thermo2.begin(MAX31865_3WIRE);

  display_init();

  // create device id string
  uint8_t mac[6];
  WiFi.macAddress(mac);
  sprintf(idstr, "esp%02X%02X%02X", mac[3], mac[4], mac[5]);

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  display_connecting();
  int cnt = 0;
  while (WiFi.status() != WL_CONNECTED) {
    cnt ++;
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/test.svg", drawGraph);
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });
  server.on("/data.json", handleData);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  uint32_t now = millis();
  
  server.handleClient();
  MDNS.update();

  btn.poll();
  if (btn.press) Serial.println("Pressed");
  if (btn.release) Serial.println("Released");

  static uint32_t tempT = TEMP_READ_PERIOD_MS;
  if ((now - tempT) >= TEMP_READ_PERIOD_MS) {
    tempT = now;  
    for (int s=0; s<2; s++) {
      temp[s] = read_temperature(s);
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
        display_wifi();
        break;
      case 1:
        display_temp(0);
        break;
      case 2:
        display_temp(1);
        break;
      default:
        break;
    }
    
    disp ++;
    disp %= 3;
  }
}
