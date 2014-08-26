#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "notes.h"
#include "furelise.h"


void buz_freq(int freq)
{
OCR1A = (F_CPU/(2*1024))/freq;

}

void delay_ms(int n) {
  while (n--) {
   _delay_ms(1);
 }
} 

int buzzer_init(void)
{

    TCCR1B |= _BV(WGM12);
	TCCR1A |= _BV(COM1A0);
  
}
