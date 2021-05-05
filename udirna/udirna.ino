/*
    DONE: 
  1) NTP client
  2) Connect display
  3) Connect sensor
  4) Show IP and time
  5) Show temperature

    TODO: 
  6) Html temperature
    a) install LAMP (local testing)
    b) create input form
    c) send data
    d) test data
  7) Connect second sensor

  test db:
    database uzeniTest
    user udirna
    password udirna123
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <ESP8266HTTPClient.h>

#include <NTPClient.h>
#include <WiFiUdp.h>

#include <Adafruit_MAX31865.h>

#include "password.h"

#ifndef STASSID
#define STASSID "wifi_ssid"
#define STAPSK  "wifi_password"
#define KEY "123456"
#define DATAIN "789ABC"
#endif

const long utcOffsetInSeconds = 7200;

const char *ssid = STASSID;
const char *password = STAPSK;

int temp[2];

// display
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

ESP8266WebServer server(80);

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

// The value of the Rref resistor. Use 430.0 for PT100 and 4300.0 for PT1000
#define RREF      430.0
// The 'nominal' 0-degrees-C resistance of the sensor
// 100.0 for PT100, 1000.0 for PT1000
#define RNOMINAL  100.0

char idstr[16] = "";

// Use software SPI: CS, DI, DO, CLK
//Adafruit_MAX31865 thermo = Adafruit_MAX31865(D8, D7, D6, D5); // 10k pulldown on D8
Adafruit_MAX31865 thermo1 = Adafruit_MAX31865(2, 13, 12, 14);
Adafruit_MAX31865 thermo2 = Adafruit_MAX31865(0, 13, 12, 14);

const int led = 13;

void handleRoot() {
  digitalWrite(led, 1);
  char temp[400];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  snprintf(temp, 400,

"<html>\
  <head>\
    <meta http-equiv='refresh' content='5'/>\
    <title>ESP8266 Demo</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Hello from ESP8266!</h1>\
    <p>Uptime: %02d:%02d:%02d</p>\
    <p>Time: %2d:%02d:%02d</p>\
  </body>\
</html>",

           hr, min % 60, sec % 60, timeClient.getHours(), timeClient.getMinutes(), timeClient.getSeconds()
          );
  server.send(200, "text/html", temp);
  digitalWrite(led, 0);
}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void) {
  Serial.begin(115200);

  thermo1.begin(MAX31865_3WIRE);
  thermo2.begin(MAX31865_3WIRE);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))// Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));

  // show initial display buffer contents on the screen
  display.display();

  pinMode(led, OUTPUT);
  digitalWrite(led, 0);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
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
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
  
  timeClient.begin();

  uint8_t mac[6];
  WiFi.macAddress(mac);
  sprintf(idstr, "d1mini%02X%02X%02X", mac[3], mac[4], mac[5]);

  display_wifi();
}

int read_temperature(uint8_t s) {
  if (s > 1) return 0;
  
  float t;
  uint16_t rtd;
  if (s == 0) {
    rtd = thermo1.readRTD();
    t = thermo1.temperature(RNOMINAL, RREF);
  }
  else {
    rtd = thermo2.readRTD();
    t = thermo2.temperature(RNOMINAL, RREF);
  }

  int it = round(t * 10);

  Serial.print("Temperature ");
  Serial.print(s + 1);
  Serial.print(" ");
  Serial.print(t);
  Serial.println(" C");

  return it;
}

bool push_data_to_server(float t1, float t2) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    int it1, dt1, it2, dt2;
    it1 = t1 / 10;
    dt1 = t1 - it1 * 10;
    it2 = t2 / 10;
    dt2 = t2 - it2 * 10;
    char post[128];
    //sprintf(post, "key=blablabla&data=%d.%d", it, dt);
    //http.begin("http://192.168.88.230/test/data_input.php");
    //http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    sprintf(post, "{\"key\":\"%s\", \"idstr\":\"%s\", \"data\":[\"%d.%d\", \"%d.%d\"]}", KEY, idstr, it1, dt1, it2, dt2);
    http.begin(DATAIN);
    http.addHeader("Content-Type", "application/json");
    int httpCode = http.POST(post);
    String payload = http.getString();
    http.end();

    Serial.print("POST ");
    Serial.print(post);
    Serial.print(" ");
    Serial.print(it1);
    Serial.print(".");
    Serial.print(dt1);
    Serial.print(" ");
    Serial.print(it2);
    Serial.print(".");
    Serial.print(dt2);
    Serial.print(" ");
    Serial.println(httpCode);
    Serial.println(payload);

    if (payload == "OK")
      return true;
  }
  return false;
}

void loop(void) {
  timeClient.update();
  server.handleClient();
  MDNS.update();

  static uint32_t temp_t = 30001;
  uint32_t now = millis();
  static uint8_t s = 0;
  if ((now - temp_t) > 30000) {
    temp[s] = read_temperature(s++);
    if (s == 2) {
      push_data_to_server(temp[0], temp[1]);
      temp_t = now;
      s = 0;
    }
  }

  static int disp = 1;
  static uint32_t disp_t = 0;
  if ((now - disp_t) > 5000) {
    disp_t = now;
    switch (disp) {
      case 0: // WIFI
        display_wifi();
        disp ++;
      break;
      case 1: // TIME
        display_time();
        disp ++;
      break;
      case 2: // TEMP
        display_temp(0);
        disp = 0;
      break;
    }
  }
}
