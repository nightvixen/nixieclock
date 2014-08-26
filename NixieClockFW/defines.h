#define MODE_TIME 0
#define MODE_SALARM0 1
#define MODE_SALARM1 2
#define MODE_SDATE   3

void eeprom_write_alarm(uint8_t idx, uint16_t val);

uint16_t eeprom_read_alarm(uint8_t idx);
