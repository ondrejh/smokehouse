// The value of the Rref resistor. Use 430.0 for PT100 and 4300.0 for PT1000
#define RREF      430.0
// The 'nominal' 0-degrees-C resistance of the sensor
// 100.0 for PT100, 1000.0 for PT1000
#define RNOMINAL  100.0

int read_temperature(uint8_t s) {
  if (s > 1) return 0;
  
  float t;
  uint16_t rtd;
  if (s == 0) {
    rtd = thermo1.readRTD();
    t = thermo1.temperature(RNOMINAL, RREF);
  }
  else {
    rtd = thermo2.readRTD();
    t = thermo2.temperature(RNOMINAL, RREF);
  }

  int it = round(t * 10);

  Serial.print("Temperature ");
  Serial.print(s + 1);
  Serial.print(" ");
  Serial.print(t);
  Serial.println(" C");

  return it;
}
