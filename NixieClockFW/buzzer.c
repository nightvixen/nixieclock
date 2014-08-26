#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
//#include "notes.h"
//#include "furelise.h"


void buz_freq(int freq)
{


	TCNT1 =0;
	OCR1A =0;
	if (freq == 0)
	{
	DDRB &= ~_BV(PB1);;
	OCR1A=0;
	}
	else
	{
	DDRB |= _BV(PB1);
	OCR1A = (F_CPU/freq);
	}
}

void delay_ms(int n) {
  while (n--) {
   _delay_ms(1);
 }
} 

void buzzer_init(void)
{

    TCCR1B |= _BV(WGM12)|_BV(CS10);
	TCCR1A |= _BV(COM1A0);
  
}
