#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <ESP8266HTTPClient.h>
#include <PubSubClient.h>

#include <EEPROM.h>

#include <Adafruit_MAX31865.h>

#include "button.h"
#include "webi.h"

#define TEMP_READ_PERIOD_MS 5000
#define DISPLAY_CYCLE_PERIOD_MS 10000
#define BUTTON_PRESS_CYCLE_PAUSE_MS 15000
#define SERVER_SEND_PERIOD_MS 15000
#define MQTT_SEND_PERIOD_MS 5000

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
bool valid[2];
uint32_t uptim = 0;

//String key = SECRET_KEY;
char idstr[16] = "";

#define EEPROM_SIZE 1024

// configuration data
#define EEPROM_CONF_ADDR 0
#define URL_STR_MAX 128
#define CAPT_STR_MAX 32
#define IP_STR_MAX 32
typedef struct {
  char ssid[SSID_STR_MAX + 1];
  char pwd[PWD_STR_MAX + 1];
  char url[URL_STR_MAX + 1];
  char key[PWD_STR_MAX + 1];
  char capt[CAPT_STR_MAX + 1];
  char mqtt_ip[IP_STR_MAX + 1];
  char mqtt_topic[URL_STR_MAX + 1];
  char mqtt_user[CAPT_STR_MAX + 1];
  char mqtt_pwd[PWD_STR_MAX + 1];
} config_t;

config_t conf;

// home assistant mqtt
const int MQTT_PORT = 1883;

// Use software SPI: CS, DI, DO, CLK
//Adafruit_MAX31865 thermo = Adafruit_MAX31865(D8, D7, D6, D5); // 10k pulldown on D8
Adafruit_MAX31865 thermo1 = Adafruit_MAX31865(2, 13, 12, 14);
Adafruit_MAX31865 thermo2 = Adafruit_MAX31865(0, 13, 12, 14);

Button btn = Button(btnPin);
bool ap_mode = false;
bool server_status = false;
bool mqtt_status = false;

WiFiClient client;
WiFiClientSecure clientSecure;

PubSubClient mqttClient(client);
IPAddress brokerIP;

void mqtt_reconnect() {
  // MQTT reconnect loop
  if (!mqttClient.connected()) {
    Serial.print("Connecting to MQTT... ");

    // CONNECT WITH USER + PASSWORD
    if (conf.mqtt_user[0]!='\0') {
      //if (mqttClient.connect(idstr, mqttUser, mqttPassword)) {
      if (mqttClient.connect(idstr, conf.mqtt_user, conf.mqtt_pwd)) {
        Serial.println("connected!");
      }
      else {
        Serial.print("failed, rc=");
        Serial.println(mqttClient.state());
      }
    }

    // CONNECT (NO USER)
    else {
      if (mqttClient.connect(idstr)) {
        Serial.println("connected!");
      }
      else {
        Serial.print("failed, rc=");
        Serial.println(mqttClient.state());
      }
    }
  }
}

bool push_data_to_server() {
  if (!ap_mode && (WiFi.status() == WL_CONNECTED) && (conf.url[0] != '\0')) {
    Serial.println(millis());
    Serial.print("POST ");
    Serial.println(conf.url);
    uint32_t tElap = micros();
    
    //WiFiClientSecure client;
    //client.setInsecure();
    HTTPClient http;
    char st1[16], st2[16], sup[16];
    if (valid[0]) {
      int it1 = temp[0] / 10;
      int dt1 = (temp[0] >= 0) ? (temp[0] - it1 * 10) : (-temp[0] + it1 * 10);
      sprintf(st1, "%d.%d", it1, dt1);
    }
    else {
      sprintf(st1, "null");
    }
    if (valid[1]) {
      int it2 = temp[1] / 10;
      int dt2 = (temp[1] >= 0) ? (temp[1] - it2 * 10) : (-temp[1] + it2 * 10);
      sprintf(st2, "%d.%d", it2, dt2);
    }
    else {
      sprintf(st2, "null");
    }
    sprintf(sup, "%d", uptim);
    char post[256];
    sprintf(post, "{\"id\":\"%s\", \"key\":\"%s\", \"data\":{\"temp1\": \"%s\", \"temp2\": \"%s\", \"uptim\": \"%s\"}}", idstr, conf.key, st1, st2, sup);
    http.begin(clientSecure, conf.url);
    http.addHeader("Content-Type", "application/json");
    int httpCode = http.POST(post);
    String payload = http.getString();

    http.end();

    tElap = micros() - tElap;
    Serial.print("TIME ");
    Serial.println(tElap);
    Serial.print("DATA ");
    Serial.println(post);
    Serial.print(httpCode);
    Serial.print(" ");
    Serial.println(payload);

    if (httpCode == 200) {
      //Serial.println("True");
      return true;
    }
  }
  return false;
}

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

      // Resolve broker hostname
      if (conf.mqtt_ip[0] != '\0') {
        //if (WiFi.hostByName("homeassistant.local", brokerIP)) {
        if (WiFi.hostByName(conf.mqtt_ip, brokerIP)) {
          Serial.print("MQTT server: ");
          Serial.print(brokerIP);
          Serial.print(":");
          Serial.println(MQTT_PORT);
          mqttClient.setServer(brokerIP, MQTT_PORT);
        } else {
          Serial.println("Resolve failed");
        }
      }
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
  //server.on(style_name, handleStyle);
  //server.on(script_name, handleScript);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");

  clientSecure.setInsecure();
}

void loop(void) {
  uint32_t now = millis();
  static bool refresh = false;

  static uint32_t uptim_ms;
  while ((now - uptim_ms) > 1000) {
    uptim_ms += 1000;
    uptim ++;
  }
  
  server.handleClient();
  //MDNS.update();

  btn.poll();
  if (btn.press) Serial.println("Pressed");
  if (btn.release) Serial.println("Released");

  static uint32_t tempT = TEMP_READ_PERIOD_MS;
  static int tempCh = 0;
  if ((now - tempT) >= TEMP_READ_PERIOD_MS) {
    int t;
    if (read_temperature(tempCh, &t) == 0) {
      temp[tempCh] = t;
      valid[tempCh] = true;
    }
    else {
      valid[tempCh] = false;
    }
    tempCh ++;
    if (tempCh >= 2) {
      tempT = now;
      tempCh = 0;
    }
  }

  static uint32_t dispT = 0;
  static int disp = 0;
  static uint32_t period = DISPLAY_CYCLE_PERIOD_MS;
  if ( ((now - dispT) >= period) || btn.press || refresh) {
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
    
    if (!refresh) {
      disp ++;
      disp %= 3;
    }
    refresh = false;
  }

  if (!ap_mode) {
    if (conf.url[0] != '\0') {
      static uint32_t pushT = now;
      if ((now - pushT) >= SERVER_SEND_PERIOD_MS) {
        if (conf.url[0] != '\0') {
          display_transfer();
          server_status = push_data_to_server();
          refresh = true;
        }
        pushT = millis();
      }
    }
    else {
      server_status = false;
    }

    if (conf.mqtt_ip[0] != '\0') {
      static uint32_t pushM = now;
      if ((now - pushM) >= MQTT_SEND_PERIOD_MS) {
        if (!mqttClient.connected()) {
          mqtt_reconnect();
        }
        if (mqttClient.connected()) {
          mqtt_status = true;
          String t1 = temp2string(temp[0], valid[0]);
          String t2 = temp2string(temp[1], valid[1]);

          // Create JSON payload
          //String json = "{";
          //json += "\"t1\": ";
          //json += t1);
          //json += ",\"t2\": ";
          //json += t2);
          //json += "}";

          // publish json
          //mqttClient.publish(mqttTopic, json.c_str());

          Serial.print("Publish: ");
          //Serial.println(json);

          // publish
          mqttClient.publish(((String)conf.mqtt_topic + "/t1").c_str(), t1.c_str());
          mqttClient.publish(((String)conf.mqtt_topic + "/t2").c_str(), t2.c_str());
          mqttClient.loop();

          Serial.print(t1);
          Serial.print(", ");
          Serial.println(t2);
        }
        else {
          mqtt_status = false;
        }
        pushM = millis();
      }
    }
    else {
      mqtt_status = false;
    }
  } // !ap_mode
  else {
    server_status = false;
    mqtt_status = false;
  }
}
