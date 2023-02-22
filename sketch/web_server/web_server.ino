#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <EEPROM.h>

#include <Adafruit_MAX31865.h>

#include "secret_password.h"
#include "button.h"
#include "webi.h"

#ifndef STASSID
#define STASSID "ssid"
#define STAPSK  "password"
#endif

#define TEMP_READ_PERIOD_MS 10000
#define DISPLAY_CYCLE_PERIOD_MS 5000
#define BUTTON_PRESS_CYCLE_PAUSE_MS 15000

#define AP_PASSWORD "temPr321"
IPAddress ap_ip(192,168,42,1);
IPAddress ap_mask(255,255,255,0);

#define SSID_STR_MAX 32
#define PWD_STR_MAX 16

char ssid[SSID_STR_MAX + 1];// = STASSID;
char password[PWD_STR_MAX];// = STAPSK;

char disp_ssid[32];
IPAddress disp_ip;

ESP8266WebServer server(80);

const int led = 13;
const int btnPin = 16;

int temp[2];

String key = SECRET_KEY;
char idstr[16] = "";

#define EEPROM_SIZE 1024

// configuration data
#define EEPROM_CONF_ADDR 0
#define URL_STR_MAX 128
#define CAPT_STR_MAX 32
typedef struct {
  char ssid[SSID_STR_MAX + 1];
  char pwd[PWD_STR_MAX + 1];
  char url[URL_STR_MAX + 1];
  char key[PWD_STR_MAX + 1];
  char capt[CAPT_STR_MAX + 1];
} config_t;

config_t conf;


// Use software SPI: CS, DI, DO, CLK
//Adafruit_MAX31865 thermo = Adafruit_MAX31865(D8, D7, D6, D5); // 10k pulldown on D8
Adafruit_MAX31865 thermo1 = Adafruit_MAX31865(2, 13, 12, 14);
Adafruit_MAX31865 thermo2 = Adafruit_MAX31865(0, 13, 12, 14);

Button btn = Button(btnPin);
bool ap_mode = false;

void setup(void) {
  Serial.begin(115200);
  delay(300);
  Serial.println();

  pinMode(led, OUTPUT);
  digitalWrite(led, 0);

  thermo1.begin(MAX31865_3WIRE);
  thermo2.begin(MAX31865_3WIRE);

  // init eeprom data
  init_eeprom(EEPROM_SIZE);
  if (eeload(EEPROM_CONF_ADDR, &conf, sizeof(conf)) < 0) {
    Serial.println("No valid configuration in EEPROM. Using default.");
    conf.ssid[0] = 0;
    conf.pwd[0] = 0;
    conf.url[0] = 0;
    conf.key[0] = 0;
    strcpy(conf.capt, "Teploměr\0");
    eesave(EEPROM_CONF_ADDR, &conf, sizeof(conf));
    ap_mode = true;
  }
  else {
    if (conf.ssid[0] == 0) {
      Serial.println("WiFi not configured. Start AP mode.");
      ap_mode = true;
    }
  }

  display_init();

  // create device id string
  uint8_t mac[6];
  WiFi.macAddress(mac);
  sprintf(idstr, "Temp%02X%02X%02X", mac[3], mac[4], mac[5]);

  Serial.begin(115200);

  for (int i=0; i<16; i++) {
    btn.poll();
    if (btn.press) {
      ap_mode = true;
      break;
    }
    delay(2);
  }

  if (!ap_mode) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(conf.ssid, conf.pwd);
    Serial.println("");

    // Wait for connection
    display_connecting();
    int cnt = 0;
    while (WiFi.status() != WL_CONNECTED) {
      cnt ++;
      if (cnt > 60) {
        ap_mode = true;
        break;
      }
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    if (!ap_mode) {
      strncpy(disp_ssid, conf.ssid, 32);
      disp_ip = WiFi.localIP();
      Serial.print("Connected to ");
      Serial.println(disp_ssid);
      Serial.print("IP address: ");
      Serial.println(disp_ip);//WiFi.localIP());
    }
    else {
      Serial.print("Can't connect to ");
      Serial.println(conf.ssid);
    }
  }
  if (ap_mode) {
    display_apmode();
    Serial.println("Configuring access point.");
    /* You can remove the password parameter if you want the AP to be open. */
    strncpy(disp_ssid, idstr, 32);
    WiFi.softAPConfig(ap_ip, ap_ip, ap_mask);
    WiFi.softAP(idstr, AP_PASSWORD);

    disp_ip = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(disp_ip);
  }

  if (MDNS.begin(idstr)) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/index.html", handleRoot);
  server.on(config_name, handleConfig);
  server.on("/data.json", handleData);
  server.on("/config.json", handleConf);
  server.on("/set.php", HTTP_POST, handleSet);
  server.on(favicon_name, handleFavicon);
  server.on(style_name, handleStyle);
  server.on(script_name, handleScript);
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
        display_wifi(disp_ssid, disp_ip);
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
