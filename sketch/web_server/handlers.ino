void jsonHeader() {
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Cache-Control", "no-cache");
  server.sendHeader("Content-type", "application/json");
}

void handleRoot() {
  server.send(200, "text/html", index_html);
}

void handleSet() {
  char msg[16];
  bool res = false;

  // set caption
  if (server.hasArg("capt")) {
    server.arg("capt").toCharArray(conf.capt, CAPT_STR_MAX);
    conf.capt[CAPT_STR_MAX] = '\0';
    res = true;
    Serial.println("Setting caption:");
    Serial.print("  "); Serial.println(conf.capt);
  }

  // set WiFi
  if (server.hasArg("ssid") && server.hasArg("pwd")) {
    server.arg("ssid").toCharArray(conf.ssid, SSID_STR_MAX);
    conf.ssid[SSID_STR_MAX] = '\0';
    server.arg("pwd").toCharArray(conf.pwd, PWD_STR_MAX);
    conf.pwd[PWD_STR_MAX] = '\0';
    res = true;
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
    res = true;
    Serial.println("Setting server connection:");
    Serial.print("  "); Serial.println(conf.url);
    Serial.print("  "); Serial.println(conf.key);
  }

  if (res) {
    eesave(EEPROM_CONF_ADDR, &conf, sizeof(conf));
  }

  sprintf(msg, res ? "OK" : "ERROR");
  server.send(200, "text/html", msg);
}

void handleData() {
  String msg = "{\"key\":\"";
  msg += key;
  msg += "\",\"idstr\":\"";
  msg += idstr;
  msg += "\",\"caption\":\"";
  msg += conf.capt;
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

  jsonHeader();
  server.send(200, "application/json", msg);
}

void handleConf() {
  int p;
  char msg[512];

  p = sprintf(msg, "{\"ssid\":\"%s\",", conf.ssid);
  p += sprintf(&msg[p], "\"url\":\"%s\",", conf.url);
  p += sprintf(&msg[p], "\"caption\":\"%s\",", conf.capt);
  p += sprintf(&msg[p], "\"wifi\":%s,", ap_mode ? "false" : "true");
  p += sprintf(&msg[p], "\"server\":%s}", "false");

  jsonHeader();
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

void handleFavicon() {
  server.send(200, "image/x-icon", favicon_bin, sizeof(favicon_bin));
}

void handleStyle() {
  server.send(200, "text/css", style_bin, sizeof(style_bin));
}

void handleScript() {
  server.send(200, "application/javascript", script_bin, sizeof(script_bin));
}

void handleConfig() {
  server.send(200, "text/html", config_html);
}
