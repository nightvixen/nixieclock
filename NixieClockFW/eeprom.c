#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <util/delay.h>

#define EEPROM_SIZE	512

#define EEPROM_SIZE	512
unsigned char bin(unsigned char dec);
unsigned char bcd (unsigned char data);
uint16_t eeprom_read_alarm(uint8_t idx);

void eeprom_write_alarm(uint8_t idx, uint16_t val)
{
	uint16_t i=0,snum=0;
	uint8_t num1=0,num2=0;
	num1 = bin(val / 100);
	num2 = bin(val % 100);

	
	for(i=0; i < EEPROM_SIZE; i++)
	{
		if(eeprom_read_byte((uint8_t*)i) == 0xFF) break;
		if( i == EEPROM_SIZE-1) { i=0; eeprom_write_byte((uint8_t*)i, 0xFF); break; }
	}

	if(idx  < 1)
	{
	snum = eeprom_read_alarm(0);

	eeprom_update_byte((uint8_t*)i, num1);
	i++;
	eeprom_update_byte((uint8_t*)i, num2);
	i++;
	eeprom_update_byte((uint8_t*)i, bin(snum / 100));
	i++;
	eeprom_update_byte((uint8_t*)i, bin(snum % 100));
	}
	else
	{
	snum = eeprom_read_alarm(1);
	eeprom_update_byte((uint8_t*)i, bin(snum / 100));
	i++;
	eeprom_update_byte((uint8_t*)i, bin(snum % 100));
	i++;
	eeprom_update_byte((uint8_t*)i, num1);
	i++;
	eeprom_update_byte((uint8_t*)i, num2);
	}


	if(i+1 < EEPROM_SIZE-1)
		{
		i++;
		eeprom_update_byte((uint8_t*)i, 0xFF);
		}
	
}

uint16_t eeprom_read_alarm(uint8_t idx)
{
	uint16_t i=0; uint8_t num1=0,num2=0;

	for(i=0; i < EEPROM_SIZE; i++)
	{
		if(eeprom_read_byte((uint8_t*)i) == 0xFF) break;
		//OFF scan_led();
	}

	if(i == 0)
	{
		i=EEPROM_SIZE-1;
		if(idx > 0) i=i-2;
		num1 = eeprom_read_byte((uint8_t*)i);
		i--;
		num2 = eeprom_read_byte((uint8_t*)i);
	}

	else

	{
		if((idx > 0) && (i > 0))  i=i-2;
		i--;
		num1 = eeprom_read_byte((uint8_t*)i);
		i--;
		num2 = eeprom_read_byte((uint8_t*)i);
	}
	if ((num1 == 0xff ) && (num2 == 0xff))
	return 0;
	else
	return ((bcd(num2) * 100)+bcd(num1));
	
}
