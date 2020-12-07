void display_wifi() {
  display.clearDisplay();
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  
  char buff[32];
  int blen;
  IPAddress ip = WiFi.localIP();
  sprintf(buff, "IP: %d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
  blen = strlen(buff);
  display.setCursor(64 - blen*3, 20);     // Start at top-left corner
  display.write(buff);

  sprintf(buff, "SSID: %s", ssid);
  blen = strlen(buff);
  display.setCursor(64 - blen*3, 4);     // Start at top-left corner
  display.write(buff);  

  display.display();
}

void display_time() {
  if (timeClient.getEpochTime() < 1000)
    return;
    
  display.clearDisplay();
  display.setTextSize(3);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  
  char buff[16];
  int blen;
  sprintf(buff, "%2d:%02d", timeClient.getHours(), timeClient.getMinutes());
  blen = strlen(buff);
  display.setCursor(64 - blen*9, 10);     // Start at top-left corner
  display.write(buff);

  display.display();
}

void display_temp() {
  display.clearDisplay();
  display.setTextSize(3);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  
  char buff[16];
  int blen;
  int t = temp / 10;
  int dt;
  if (temp >= 0)
    dt = temp - t * 10;
  else
    dt = -temp + t*10;
  sprintf(buff, "%d.%01d C", t,dt);
  blen = strlen(buff);
  int xs = 64 - blen*9;
  display.setCursor(xs, 10);     // Start at top-left corner
  display.write(buff);

  display.setTextSize(2);
  display.setCursor(xs + blen*18 - 32, 5);
  display.write("o");

  display.display();
  
}
