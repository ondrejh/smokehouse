// --- eeprom functions ---

// TODO: replace checksum with kinda CRC

// init
void init_eeprom(uint32_t siz) {
  EEPROM.begin(siz);
}

// save object to eeprom (with checksum)
int eesave(uint32_t adr, void *obj, uint32_t siz) {
  int loc = siz + 1;
  uint8_t *o;
  o = (uint8_t*) obj;
  if ((adr + loc) < EEPROM_SIZE) {
    uint8_t chsum = 0;
    uint32_t a = adr;
    for (int i = 0; i < siz; i++) {
      uint8_t b = *o++;
      EEPROM.write(a++, b);
      chsum += b;
    }
    chsum = 0x00 - chsum;
    EEPROM.write( a, chsum );
    if (!EEPROM.commit())
      return -2;
  }
  else
    return -1; // not enough eeprom space
  return siz; // ok
}

// load object from eeprom (with checksum)
int eeload(uint32_t adr, void *obj, uint32_t siz) {
  int loc = siz + 1;
  uint8_t *o;
  o = (uint8_t*) obj;
  if ((adr + loc) < EEPROM_SIZE) {
    uint8_t chsum = 0;
    uint32_t a = adr;
    for (int i = 0; i < loc; i++)
      chsum += EEPROM.read( a++ );
    if (chsum != 0)
      return -2; // incorrect chsum
    a = adr;
    for (int i = 0; i < siz; i++)
      *o++ = EEPROM.read( a++ );
  }
  else 
    return -1; // not enough eepro space
  return siz; // ok
}
