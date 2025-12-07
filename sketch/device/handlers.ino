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

  // set MQTT
  if (server.hasArg("mqtt_ip")) {
    server.arg("mqtt_ip").toCharArray(conf.mqtt_ip, URL_STR_MAX);
    conf.ssid[URL_STR_MAX] = '\0';
    if (server.hasArg("mqtt_topic")) {
      server.arg("mqtt_topic").toCharArray(conf.mqtt_topic, URL_STR_MAX);
      conf.mqtt_topic[URL_STR_MAX] = '\0';
    }
    else {
      // default topic
      sprintf(conf.mqtt_topic, "home/%s", idstr);
    }
    if (server.hasArg("mqtt_usr") && server.hasArg("mqtt_pwd")) {
      server.arg("mqtt_usr").toCharArray(conf.mqtt_user, CAPT_STR_MAX);
      server.arg("mqtt_pwd").toCharArray(conf.mqtt_pwd, PWD_STR_MAX);
    }
    else {
      // default no user
      conf.mqtt_user[0] = '\0';
    }
    res = true;
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

  if (server.hasArg("mqtt_ip")) {
    
  }

  if (res) {
    eesave(EEPROM_CONF_ADDR, &conf, sizeof(conf));
  }

  sprintf(msg, res ? "OK" : "ERROR");
  server.send(200, "text/html", msg);
}

void handleData() {
  //uint32_t tElap = micros();
  String msg = "{\"caption\":\"";
  msg += conf.capt;
  msg += "\",\"data\":[";
  for (int i=0; i<2; i++) {
    if (i > 0)
      msg += ",";
    if (valid[i]) {
      int t = temp[i] / 10;
      int dt;
      if (temp[i] >= 0)
        dt = temp[i] - t * 10;
      else
        dt = -temp[i] + t * 10;
      msg += "\"";
      msg += t;
      msg += ".";
      msg += dt;
      msg += "\"";
    }
    else {
      msg += "\"null\"";      
    }
  }
  msg += "]}";
  /*tElap = micros() - tElap;
  Serial.print("Data ");
  Serial.println(tElap);*/

  jsonHeader();
  server.send(200, "application/json", msg);
}

void handleConf() {
  int p;
  char msg[512];

  p = sprintf(msg, "{\"ssid\":\"%s\",", conf.ssid);
  p += sprintf(&msg[p], "\"url\":\"%s\",", conf.url);
  p += sprintf(&msg[p], "\"caption\":\"%s\",", conf.capt);
  p += sprintf(&msg[p], "\"mqtt_ip\":\"%s\",", conf.mqtt_ip); // mqtt_ip (should be url)
  // mqtt_topic
  // mqtt_usr
  // mqtt_pwd
  p += sprintf(&msg[p], "\"wifi\":%s,", ap_mode ? "false" : "true");
  p += sprintf(&msg[p], "\"server\":%s}", server_status ? "true" : "false");

  // caption
  String s = "{\"caption\":\"" + (String)conf.capt + "\",";
  
  // wifi
  s += "\"ssid\":\"" + String(conf.ssid) + "\",";

  // mqtt
  if (conf.mqtt_ip[0] != '\0') {
    s += "\"mqtt_ip\":\"" + String(conf.mqtt_ip) + "\",";
    s += "\"mqtt_topic\":\"" + String(conf.mqtt_topic) + "\",";
    if (conf.mqtt_user[0] != '\0') {
      s += "\"mqtt_usr\":\"" + String(conf.mqtt_user) + "\",";
    }
  }

  // remote server
  s += "\"url\":\"" + String(conf.url) + "\",";

  // online statuses
  s += "\"wifi\":" + String(ap_mode ? "false" : "true") + ",";
  s += "\"server\":" + String(server_status ? "true" : "false") + ",";
  s += "\"mqtt\":" + String(mqtt_status ? "true" : "false") + "}";

  jsonHeader();
  //server.send(200, "application/json", msg);
  server.send(200, "application/json", s.c_str());
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

/*void handleStyle() {
  server.send(200, "text/css", style_bin, sizeof(style_bin));
}

void handleScript() {
  server.send(200, "application/javascript", script_bin, sizeof(script_bin));
}*/

void handleConfig() {
  server.send(200, "text/html", config_html);
}
