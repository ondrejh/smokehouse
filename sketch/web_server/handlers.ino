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
    <img src=\"/test.svg\" />\
  </body>\
</html>",

           hr, min % 60, sec % 60
          );
  server.send(200, "text/html", temp);
  digitalWrite(led, 0);
}

void handleSet() {
  digitalWrite(led, HIGH);

  char msg[16];
  bool res = false;

  // set WiFi
  if (server.hasArg("ssid") && server.hasArg("pwd")) {
    server.arg("ssid").toCharArray(conf.ssid, SSID_STR_MAX);
    conf.ssid[SSID_STR_MAX] = '\0';
    server.arg("pwd").toCharArray(conf.pwd, PWD_STR_MAX);
    conf.pwd[PWD_STR_MAX] = '\0';
    // save to eeprom
    eesave(EEPROM_CONF_ADDR, &conf, sizeof(conf));
    // OK
    res = true;
    //
    Serial.println("Setting WiFi:");
    Serial.print("  "); Serial.println(conf.ssid);
    Serial.print("  "); Serial.println(conf.pwd);
  }

  // set server
  if (server.hasArg("url") && server.hasArg("key")) {
    server.arg("url").toCharArray(conf.url, URL_STR_MAX);
    conf.url[URL_STR_MAX] = '\0';
    server.arg("key").toCharArray(conf.key, PWD_STR_MAX);
    conf.key[URL_STR_MAX] = '\0';
    eesave(EEPROM_CONF_ADDR, &conf, sizeof(conf));
    res = true;
    Serial.println("Setting server connection:");
    Serial.print("  "); Serial.println(conf.url);
    Serial.print("  "); Serial.println(conf.key);
  }

  sprintf(msg, res ? "OK" : "ERROR");
  server.send(200, "text/html", msg);
  
  digitalWrite(led, LOW);
}

void handleData() {
  String msg = "{\"key\":\"";
  msg += key;
  msg += "\",\"idstr\":\"";
  msg += idstr;
  msg += "\",\"data\":[";
  for (int i=0; i<2; i++) {
    int t = temp[i] / 10;
    int dt;
    if (temp[i] >= 0)
      dt = temp[i] - t * 10;
    else
      dt = -temp[i] + t * 10;
    if (i > 0)
      msg += ",";
    msg += "\"";
    msg += t;
    msg += ".";
    msg += dt;
    msg += "\"";
  }
  msg += "]}";

  server.send(200, "application/json", msg);
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

void drawGraph() {
  String out;
  out.reserve(2600);
  char temp[70];
  out += "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"400\" height=\"150\">\n";
  out += "<rect width=\"400\" height=\"150\" fill=\"rgb(250, 230, 210)\" stroke-width=\"1\" stroke=\"rgb(0, 0, 0)\" />\n";
  out += "<g stroke=\"black\">\n";
  int y = rand() % 130;
  for (int x = 10; x < 390; x += 10) {
    int y2 = rand() % 130;
    sprintf(temp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke-width=\"1\" />\n", x, 140 - y, x + 10, 140 - y2);
    out += temp;
    y = y2;
  }
  out += "</g>\n</svg>\n";

  server.send(200, "image/svg+xml", out);
}
