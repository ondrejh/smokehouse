#include <Adafruit_MAX31865.h>

#define BUFLEN 128

#define MODULE 150 // ms
#define DOT 1
#define LINE 3
#define PAUSE 1
#define LETTER_PAUSE 3
#define WORD_PAUSE 7

int temp[2];

const int led = 13;
const int morse = 16;

// Use software SPI: CS, DI, DO, CLK
//Adafruit_MAX31865 thermo = Adafruit_MAX31865(D8, D7, D6, D5); // 10k pulldown on D8
Adafruit_MAX31865 thermo1 = Adafruit_MAX31865(2, 13, 12, 14);
Adafruit_MAX31865 thermo2 = Adafruit_MAX31865(0, 13, 12, 14);

void setup(void) {
  Serial.begin(115200);

  thermo1.begin(MAX31865_3WIRE);
  thermo2.begin(MAX31865_3WIRE);

  display_init();

  pinMode(led, OUTPUT);
  digitalWrite(led, 0);

  pinMode(morse, OUTPUT);
  digitalWrite(morse, 0);

  delay(1000);
}

void loop(void) {
  for (int s=0; s<2; s++) {
    temp[s] = read_temperature(s);
  }

  static int disp = 0;
  switch (disp) {
    case 0: // TEMP 1
      display_temp(0);
      disp ++;
    break;
    case 1: // TEMP 2
      display_temp(1);
      //testdrawchar();
      disp = 0;
    break;
  }

  static int morse_st = 0;
  static uint32_t morse_t = 0;
  static char sent[BUFLEN];
  int p = sprintf(sent, "T1");
  int t, d;
  if (temp[0]<0) {
    t = -temp[0] / 10;
    d = -temp[0] % 10;
    p += sprintf(&sent[p], "M");
  }
  else {
    t = temp[0] / 10;
    d = temp[0] % 10;
    p += sprintf(&sent[p], "P");
  }
  p = 3;
  p += sprintf(&sent[p], "%d", t);
  p += sprintf(&sent[p], "D");
  p += sprintf(&sent[p], "%d", d);
  p += sprintf(&sent[p], "C ");
  p += sprintf(&sent[p], "T2");
  if (temp[1]<0) {
    t = -temp[1] / 10;
    d = -temp[1] % 10;
    p += sprintf(&sent[p], "M");
  }
  else {
    t = temp[1] / 10;
    d = temp[1] % 10;
    p += sprintf(&sent[p], "P");
  }
  p += sprintf(&sent[p], "%d", t);
  p += sprintf(&sent[p], "D");
  p += sprintf(&sent[p], "%d", d);
  p += sprintf(&sent[p], "C");
  sent[p] = 0;

  Serial.println(sent);

  for (int i=0; i<BUFLEN; i++) {
    char c = sent[i];
    uint8_t mc;
    int len = morse_encode_char(c, &mc);
    if (len > 0) {
      Serial.print(c);
      Serial.print(' ');
      uint8_t mask = 1 << len;
      for (int j=0; j<len; j++) {
        mask >>= 1;
        bool b = ((mc & mask) != 0);
        Serial.print(b ? '-' : '.');
        digitalWrite(morse, 1);
        delay(MODULE * (b ? LINE : DOT));
        digitalWrite(morse, 0);
        delay(MODULE * PAUSE);
        //mask >>= 1;
      }
      Serial.println();
    }
    
    delay(MODULE * (LETTER_PAUSE - PAUSE));
    
    if ((c == 0) || (c == ' ')) {
      delay(MODULE * (WORD_PAUSE - LETTER_PAUSE - PAUSE));
    }
    
    if (c==0)
      break;
  }
}
