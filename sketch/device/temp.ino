// The value of the Rref resistor. Use 430.0 for PT100 and 4300.0 for PT1000
#define RREF      430.0
// The 'nominal' 0-degrees-C resistance of the sensor
// 100.0 for PT100, 1000.0 for PT1000
#define RNOMINAL  100.0

#define AVGLEN 8

int read_temperature(uint8_t s, int *it) {
  static float f1[AVGLEN];
  static float f2[AVGLEN];
  static int p1 = 0, p2 = 0;
  static bool first1 = true, first2 = true;
  bool ignore = false;
  
  if (s > 1) return 0;
  
  float t;
  uint16_t rtd;
  if (s == 0) {
    rtd = thermo1.readRTD();
    t = thermo1.temperature(RNOMINAL, RREF);
    if ((t > 900.0) || (t < -200.0)) {
      // ignore result
      // adc or sensor is disconnected
      ignore = true;
      first1 = true;
    }
    else {
      if (first1) {
        for (int i=0; i<AVGLEN; i++)
          f1[i] = t;
        first1 = false;
      }
      else {
        f1[p1] = t;
        p1 += 1;
        p1 %= AVGLEN;
      }
      t = 0.0;
      for (int i=0; i<AVGLEN; i++)
        t += f1[i];
      t /= AVGLEN;
    }
  }
  else {
    rtd = thermo2.readRTD();
    t = thermo2.temperature(RNOMINAL, RREF);
    if ((t > 900.0) || (t < -200.0)) {
      // ignore result
      // adc or sensor is disconnected
      ignore = true;
      first2 = true;
    }
    else {
      if (first2) {
        for (int i=0; i<AVGLEN; i++)
          f2[i] = t;
        first2 = false;
      }
      else {
        f2[p2] = t;
        p2 += 1;
        p2 %= AVGLEN;
      }
      t = 0.0;
      for (int i=0; i<AVGLEN; i++)
        t += f2[i];
      t /= AVGLEN;
    }
  }

  *it = round(t * 10);

  /*Serial.print("Temperature ");
  Serial.print(s + 1);
  Serial.print(" ");
  Serial.print(t);
  Serial.print(" C ");
  Serial.println(ignore ? "ERROR" : "OK");*/

  return (ignore ? -1 : 0);
}
