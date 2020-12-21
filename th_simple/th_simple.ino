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

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <ESP8266HTTPClient.h>

#include "DHT.h"

#include "password.h"

#ifndef STASSID
#define STASSID "wifi_ssid"
#define STAPSK  "wifi_password"
#define KEY     "12345678"
#endif

#define DHTPIN D5
#define DHTTYPE DHT21
#define SENSOR_ID "AM2301"

DHT dht(DHTPIN, DHTTYPE);
float humi, temp;

const char *ssid = STASSID;
const char *password = STAPSK;

ESP8266WebServer server(80);

char idstr[16] = "";

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
  </body>\
</html>",

           hr, min % 60, sec % 60
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
  
  uint8_t mac[6];
  //wifi_get_macaddr(STATION_IF, mac);
  WiFi.macAddress(mac);
  sprintf(idstr, "d1mini%02X%02X%02X", mac[3], mac[4], mac[5]);

  dht.begin();
}

int read_temperature(void) {
  Serial.print("Temperature ");
  //Serial.print(t);
  Serial.println(" C");

  return 0;
}

bool push_data_to_server(float t, float h) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    int it, dt, ih;
    it = t / 10;
    dt = t - it * 10;
    ih = int(h);
    char post[64];
    //sprintf(post, "key=blablabla&data=%d.%d", it, dt);
    //http.begin("http://192.168.88.230/test/data_input.php");
    //http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    sprintf(post, "{\"key\":\"%s\", \"idstr\":\"%s\", \"data\":{\"temp\":\"%d.%d\", \"humi\":\"%d\"}}", KEY, idstr, it, dt, ih);
    http.begin(DATAIN);
    http.addHeader("Content-Type", "application/json");
    int httpCode = http.POST(post);
    String payload = http.getString();
    http.end();

    Serial.print("POST ");
    Serial.print(post);
    Serial.print(" ");
    Serial.println(httpCode);
    Serial.println(payload);

    if (payload == "OK")
      return true;
  }
  return false;
}

void loop(void) {
  server.handleClient();
  MDNS.update();

  /*static uint32_t temp_t = 30001;
  uint32_t now = millis();
  if ((now - temp_t) > 30000) {
    temp_t = now;
    //temp = read_temperature();
    read_humitemp();
    push_data_to_server(temp, humi);
  }*/

  static uint32_t dht_timer = 0;
  static uint32_t dht_timeout = 0;
  static bool dht_status = false;
  uint32_t now = millis();
  if ((now - dht_timer) >= dht_timeout) {
    dht_timer = now;
    float v = dht_status?dht.readHumidity():dht.readTemperature();
    if (isnan(v))
      dht_timeout = 500;
    else {
      dht_timeout = dht_status?59900:100;
      // report humidity
      Serial.print(SENSOR_ID);
      Serial.print(" ");
      Serial.print(v);
      Serial.println(dht_status?" %":" *C");
      if (dht_status) humi = v;
      else temp = v;
      push_data_to_server(temp, humi);
    }
    dht_status = !dht_status;
  }
}
