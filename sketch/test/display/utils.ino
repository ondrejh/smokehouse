// json string functions

bool key_exists(String where, String key) {
  String k = key;
  if (key[0] != '"')
    k = "\"" + key + "\"";
  if (where.indexOf(k) != -1)
    return true;
  return false;
}

String strip_string(String what) {
  int b = 0;
  int e = what.length();
  for (b=0; b<e; b++) {
    if (!((what[b] == ':') || (what[b] == ' ') || (what[b] == '"')))
      break;
  }
  e--;
  for (; e>b; e--) {
    if (!((what[e] == ' ') || (what[e] == '"') || (what[e] == '}')))
      break;
  }
  return what.substring(b, e+1);
}

String find_key_simple(String where, String key) {
  String k = key;
  if (key[0] != '"')
    k = "\"" + key + "\"";
  int b = where.indexOf(k);
  if (b != -1) {
    b += k.length();
    int e = where.indexOf(",", b);
    return where.substring(b, e); 
  }
  return "";
}

// display functions

void display_temp(float temp) {
  display.clearDisplay();
  display.setTextSize(3);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  int tt = int(temp * 10);
  
  char buff[16];
  int blen;
  int t = tt / 10;
  int dt;
  if (tt >= 0)
    dt = tt - t * 10;
  else
    dt = -tt + t*10;
  sprintf(buff, "%d.%01d C", t,dt);
  blen = strlen(buff);
  int xs = 64 - blen*9;
  display.setCursor(xs, 10);     // Start at top-left corner
  display.print(buff);

  display.setTextSize(2);
  display.setCursor(xs + blen*18 - 32, 5);
  display.print("o");

  display.display(); 
}

void display_no_signal() {
  char buff[32];
  int blen;
  int tsiz;

  sprintf(buff, "---");
  
  blen = strlen(buff);
  tsiz = SCREEN_WIDTH / (blen * 6);
  if (tsiz < 1) tsiz = 1;
  if (tsiz > 3) tsiz = 3;

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  display.setTextSize(tsiz);
  display.setCursor(SCREEN_WIDTH / 2 - blen * 3 * tsiz, SCREEN_HEIGHT / 2 - tsiz * 4 + 1);
  display.print(buff);
  display.display();  
}
