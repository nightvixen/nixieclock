#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "I2C/pca2129_i2c.h"
#include "i2cmaster.h"
#include "tiny-tune/tinytune.h"
#include "tiny-tune/still_song.h"
#include "defines.h"


uint8_t counter=0, mode=0,display=0xff,tdelay=0,tmpvar0=0;
uint8_t lev_ch1, lev_ch2, lev_ch3;
uint8_t buf_lev_ch1, buf_lev_ch2, buf_lev_ch3;
uint16_t val=0,mdelay=0, alarm0=0,alarm1=0;
uint32_t sreg_data=0;
uint16_t btime=0;
unsigned char hour, min, sec,bri=255;
uint8_t fd=0, sd=0, td=0;
unsigned char dig=0;

//yiff yiff
uint8_t num;
uint8_t dots=0, alarm=0, mpos=0;
#define MLEN 71
extern  int	Djen[MLEN][2];

typedef struct 
{
	unsigned char hour, min, sec;
	unsigned char day; 
	unsigned char month;
	unsigned char year; 
	unsigned char weekday;
}date_t;

date_t date = {0,0,0,0};

#define DIG0 0b01110000000000000000000000000000;
#define DIG1 0b10110000000000000000000000000000;
#define DIG2 0b11010000000000000000000000000000;
#define DIG3 0b11100000000000000000000000000000;

#define ACT_DELAY 600

void SPI_out(char data);
void disp_num();
void SampleProccess(void);
void buz_freq(int freq);
int buzzer_init(void);


int i =0;
int e =1,g=0;

uint8_t daysInMonth(uint8_t month, uint8_t year)
{

	if (month == 4 || month == 6 || month == 9 || month == 11)  
  		return 30;  
	else if (month == 2)  
	{ 
		bool isLeapYear = (year % 4 == 0);  
  	if (isLeapYear)  
    	return 29;  
  	else  
    	return 28;  
	}  
	else  
  		return 31; 
}

/********************************************************************\
* Calculate Day of the week
* 0 -> SUN
* 6 -> SAT
\********************************************************************/
unsigned char WeekDay()
{
  int a,y,m,d;

  a = (14 - date.month) / 12;
  y = (date.year + 2000) - a;
  m = date.month + 12*a - 2;
  d = (date.day + y + y/4 - y/100 + y/400 + (31*m)/12) % 7;
  return((unsigned char)d);
} 


/* Delay for the given number of microseconds.  Assumes a 8 or 16 MHz clock.
 * Disables interrupts, which will disrupt the millis() function if used
 * too frequently. */
void delayMicroseconds(unsigned int us)
{
        uint8_t oldSREG;

        // calling avrlib's delay_us() function with low values (e.g. 1 or
        // 2 microseconds) gives delays longer than desired.
        //delay_us(us);

#if F_CPU >= 16000000L
        // for the 16 MHz clock on most Arduino boards

        // for a one-microsecond delay, simply return.  the overhead
        // of the function call yields a delay of approximately 1 1/8 us.
        if (--us == 0)
                return;

        // the following loop takes a quarter of a microsecond (4 cycles)
        // per iteration, so execute it four times for each microsecond of
        // delay requested.
        us <<= 2;

        // account for the time taken in the preceeding commands.
        us -= 2;
#else
        // for the 8 MHz internal clock on the ATmega168

        // for a one- or two-microsecond delay, simply return.  the overhead of
        // the function calls takes more than two microseconds.  can't just
        // subtract two, since us is unsigned; we'd overflow.
        if (--us == 0)
                return;
        if (--us == 0)
                return;

        // the following loop takes half of a microsecond (4 cycles)
        // per iteration, so execute it twice for each microsecond of
        // delay requested.
        us <<= 1;
   
        // partially compensate for the time taken by the preceeding commands.
        // we can't subtract any more than this or we'd overflow w/ small delays.
        us--;
#endif

        // disable interrupts, otherwise the timer 0 overflow interrupt that
        // tracks milliseconds will make us delay longer than we want.
        oldSREG = SREG;
        cli();

        // busy wait
        __asm__ __volatile__ (
                "1: sbiw %0,1" "\n\t" // 2 cycles
                "brne 1b" : "=w" (us) : "0" (us) // 2 cycles
        );

        // reenable interrupts.
        SREG = oldSREG;
}


void SPI_out(char data) 
{
SPDR=data; 
while(!(SPSR & (1<<SPIF))){} 

}

void delay_us(int n) {
  while (n--) {
   _delay_us(1);
 }
} 

void bts(void)
{
	if(btime == 0)
		btime = 1;
	
}

void SPI_uint32(uint32_t sdata)
{
	SPI_out((uint8_t)(sdata >> 8));
	SPI_out((uint8_t)(sdata >> 16));
	SPI_out((uint8_t)(sdata >> 24));
	
	
	PORTB |= _BV(PB2); 
	PORTB &= ~_BV(PB2); 
}

/*ADC Conversion Complete Interrupt Service Routine (ISR)*/
void adc_convert()
{
	if(mode != MODE_TIME) return; else bri = 200;
 
	ADCSRA  |= (1<<ADSC);					// Start ADC Conversion

	while((ADCSRA & (1<<ADIF)) != 0x10);	// Wait till conversion is complete

	val   = ADC;                         // Read the ADC Result


	if(val < 110)
//	bri= (val/4)+17; // 22
	bri=val;

	if(bri < 2)
	bri = 3;

	if(val > 849)
	bri= (val/4)+5;

	ADCSRA  |= (1 << ADIF);					// Clear ADC Conversion Interrupt Flag
	

}

 
ISR (TIMER2_OVF_vect)  
{

  
  if(alarm > 0)
  {
    if(tmpvar0 != min) { tmpvar0=min; alarm--; }
  	if(mdelay == 0)
	{
	if (mpos > (MLEN - 1)) mpos=0;
	buz_freq(Djen[mpos][0]);
	mdelay = Djen[mpos][1];
	mpos++;
	}
	else mdelay--;
   }
   else DDRB &= ~_BV(PB1); // mute dat fucking clock
  
   if(btime > 0) btime++; 
   if(mode != MODE_TIME) { if (tdelay > 3) btime = btime +5; }

   
   
}

void decode_alarm(uint8_t idx)
{
uint16_t al=0;
	switch(idx)
	{
	case 0:
	al = alarm0;
	if( al >= 5000) { dots |= _BV(5); al -= 5000; }
	break;

	case 1:
	al = alarm1;
	if( al >= 5000) { dots |= _BV(4); al -= 5000; }
	break;

	default:
	break;
	}

fd = al / 60;
sd  = al % 60;
//td holds press duration, cause we're not using third digit. =>
tdelay=0;
}

void exit_salarm(uint8_t idx)
{

			switch(idx)
			{
			case 0:
				alarm0 = (fd*60) + sd +((dots &  _BV(5)) ? 5000: 0);
				eeprom_write_alarm(idx, alarm0);
				break;
			case 1:
				alarm1 = (fd*60) + sd +((dots &  _BV(4 )) ? 5000: 0);
				eeprom_write_alarm(idx, alarm1);
				break;
			default:
			break;
			}



mode = MODE_TIME;
mpos =0;
mdelay=0;
display = 0xff;
btime=0;
}

void process_alarm(uint8_t idx)
{
	if(bit_is_clear(PINC, PC3))  
	{
	if (btime == 0) bts();
	else if(btime > ACT_DELAY) { if(mpos == 0) {if(fd < 23) fd++;  } else if (mpos == 1) {if(sd < 59) sd++;  } else if (mpos == 2){ if(idx == 0) dots |=  _BV(5); else dots |=  _BV(4);} btime=0; tdelay++;}
	}
	
	else if(bit_is_clear(PINC, PC2))  
	{
	if (btime == 0) bts();
	else if(btime > ACT_DELAY) { if(mpos == 0) {if(fd > 0) fd--;  } else if (mpos == 1) {if(sd > 0) sd--;  } else if (mpos == 2){ if(idx == 0) dots &=  ~_BV(5); else dots &=  ~_BV(4);}  btime=0; tdelay++;}
	}
	else if(bit_is_clear(PINC, PC1))  
	{
	if (btime == 0) bts();
	else if(btime > ACT_DELAY) { if(mpos < 3) mpos++; btime=0; }
	}
	else if(bit_is_clear(PINC, PC0))  
	{
	if (btime == 0) bts();
	else if(btime > ACT_DELAY) { if(mpos > 0) mpos--; btime=0; }
	}
	// td holds press duration =>
	else tdelay =0;

	switch (mpos)
	{
		case 0:
		display |= _BV(3)| _BV(2);
		if(mdelay) display |= _BV(4)| _BV(5);
		else display &= ~(_BV(4)|_BV(5));
		break;

		case 1:
		display |=  _BV(4)| _BV(5);
		if(mdelay) display |= _BV(3)| _BV(2);
		else display &= ~(_BV(3)|_BV(2));
		break;

		case 2:
		display |= _BV(3)| _BV(2)| _BV(4)| _BV(5);
		/*
		if(mdelay) dots |=  _BV(5);
		else dots &= ~_BV(5);
		*/
		break;
		case 3:
		exit_salarm(idx);
		break;
		default:
		break;
	}

}

void process_sdate(void)
{


	if(bit_is_clear(PINC, PC0) && bit_is_clear(PINC,PC3))  tdelay=0;
	else if(bit_is_clear(PINC, PC3))  
	{
	if (btime == 0) bts();
	else if(btime > ACT_DELAY) { 
			if(mpos == 0) {
				if(date.day < daysInMonth(date.month,date.year)) date.day++;  
			} 
			else if (mpos == 1) {
			if(date.month < 12) date.month++;  
			
			} 
			
			else if (mpos == 2) { 
			if(date.year < 99) date.year++;  
			} 

			else if (mpos == 3) { 
				if(date.hour < 23) date.hour++;  
			} 
			else if (mpos == 4) { 
				if(date.min < 59) date.min++;  
			} 
			else if (mpos == 5) { 
				if(date.sec < 59) date.sec++;  
			} 
			if(mpos < 3) {if(date.day > daysInMonth(date.month,date.year)) date.day = daysInMonth(date.month,date.year);}
			btime=0; tdelay++;
			}
	}
	
	else if(bit_is_clear(PINC, PC2))  
	{
	if (btime == 0) bts();
	else if(btime > ACT_DELAY) { 
			if(mpos == 0) {
				if(date.day > 1) date.day--;  
				} 
			else if (mpos == 1) {
				if(date.month > 1) date.month--;  
				} 
			else if (mpos == 2) { 
				if(date.year > 0) date.year--;  
				}
			else if (mpos ==3) { //hh
				if(date.hour > 0) date.hour--;  
			}
			else if (mpos ==4) { //mm
				if(date.min > 0) date.min--;  
			}
			else if (mpos ==5) { //ss
				if(date.sec > 0) date.sec--;  
			}
  
				btime=0; tdelay++;
			}
	}
	else if(bit_is_clear(PINC, PC1))  
	{
	if (btime == 0) bts();
	else if(btime > ACT_DELAY*2) { if(mpos < 6) mpos++; btime=0; display=0xff; }
	}
	else if(bit_is_clear(PINC, PC0))  
	{
	if (btime == 0) bts();
	else if(btime > ACT_DELAY*2) { if(mpos > 0) mpos--; btime=0; display=0xff; }
	}
	// td holds press duration =>
	else tdelay =0;

	switch (mpos)
	{
		case 0: // day
		fd = date.day;
		sd = date.month;
		td = date.year;
		dots |= (_BV(7)|_BV(6));		  // separating dots
							// blink 5 and 4
		if(mdelay) display |= _BV(5)| _BV(4);
		else display &= ~(_BV(5)|_BV(4));
		break;

		case 1: //month
		fd = date.day;
		sd = date.month;
		td = date.year;

		dots |= (_BV(7)|_BV(6));			  // separating dots
						//blink 3 and 2
		if(mdelay) display |= _BV(3)| _BV(2);
		else display &= ~(_BV(3)|_BV(2));
		break;

		case 2: // year
		dots |= (_BV(7)|_BV(6));;	
		fd = date.day;
		sd = date.month;
		td = date.year;
		if(mdelay) display |= _BV(1)| _BV(0);
		else display &= ~(_BV(1)| _BV(0));
		break;

		case 3: //hour
		fd = date.hour;
		sd = date.min;
		td = date.sec;
							//blink 5 and 4
		if(mdelay) display |= _BV(5)| _BV(4);
		else display &= ~(_BV(5)|_BV(4));
		break;
		case 4: //minute
		fd = date.hour;
		sd = date.min;
		td = date.sec;
									//blink 3 and 2
		if(mdelay) display |= _BV(3)| _BV(2);
		else display &= ~(_BV(3)|_BV(2));
		break;
		case 5: //second
		fd = date.hour;
		sd = date.min;
		td = date.sec;
									//blink 1 and 0
		if(mdelay) display |= _BV(1)| _BV(0);
		else display &= ~(_BV(1)|_BV(0));
		break;
		case 6:
		// ee_save //
		cli();
		pca2129_set_date(date.day, WeekDay(), date.month, date.year);
		pca2129_set_time(date.hour, date.min, date.sec);
		sei();
		mode = MODE_TIME;
		mpos =0;
		mdelay=0;
		btime=0;
		break;
		default:
		break;
	}


}

ISR (TIMER0_OVF_vect)  
{
uint32_t bits=0;
  counter++;

  
  if ((counter == 128)) {
  	pca2129_get_time(&hour, &min, &sec);  
  		if(mode == MODE_TIME) {
		pca2129_get_date(&date.day, &date.month, &date.year, &date.weekday);
		date.hour = hour;
		date.min  = min;
		date.sec  = sec;
		}
	}
  

  if (counter == 100)	adc_convert(); 
  if (counter < bri){ 

	if(mode == MODE_TIME)
	{
		if(sec % 2) dots |= _BV(7)| _BV(6); 
		else dots &= ~(_BV(7)|_BV(6));
		fd = hour;
		sd = min;
		td = sec;
		//td = WeekDay();
	}
	else
	{
		if((sec % 2) && !(bit_is_clear(PINC, PC2) || bit_is_clear(PINC, PC3))) mdelay = 0;
		else mdelay = 1;
	}

	if (num == 6) num =0;


	switch (num)
	{
	
	case 0:
	if(display & _BV(0))
	bits = (0 | ((uint32_t)1<< (31)) | ((uint32_t)1<< (32-7-(td%10)))); // 11th bit is dot-point
	if(dots & _BV(0)) bits |= ((uint32_t)1<< (32-6-11));
	SPI_uint32(bits);
	break;
	case 1:
	if(display & _BV(1))
	bits=(0 | ((uint32_t)1<< (30)) | ((uint32_t)1<< (32-7-((td/10) %10))));
	if(dots & _BV(1)) bits |= ((uint32_t)1<< (32-6-11));
	SPI_uint32(bits);
	break;
	case 2:
	if(display & _BV(2))
	bits=(0 | ((uint32_t)1<< (29)) | ((uint32_t)1<< (32-7-(sd%10))));
	if(dots & _BV(2)) bits |= ((uint32_t)1<< (32-6-11));
	SPI_uint32(bits);
	break;
	case 3:
	if(display & _BV(3))
	bits=(0 | ((uint32_t)1<< (28)) | ((uint32_t)1<< (32-7-((sd/10) %10))));
	if(dots & _BV(3)) bits |= ((uint32_t)1<< (32-6-11));
	SPI_uint32(bits);
	break;
	case 4:
	if(display & _BV(4))
	bits=(0 | ((uint32_t)1<< (27)) | ((uint32_t)1<< (32-7-(fd%10))));
	if(dots & _BV(4)) bits |= ((uint32_t)1<< (32-6-11));
	SPI_uint32(bits);
	break;
	case 5:
	if (dots & _BV(6))
	bits |= ((uint32_t)1<< (32-6-12))|  ((uint32_t)1<< (32-6-13));
	if (dots & _BV(7))
	bits |= ((uint32_t)1<< (32-6-14))|((uint32_t)1<< (32-6-15));
	if (display & _BV(5))
	bits |= ( ((uint32_t)1<< (26)) | ((uint32_t)1<< (32-7-((fd/10) %10))));

	if(dots & _BV(5)) bits |= ((uint32_t)1<< (32-6-11));
	if((mode == MODE_SDATE) &&(mpos < 3)) bits &= ~(((uint32_t)1<< (32-6-13)) |((uint32_t)1<< (32-6-15)));
	SPI_uint32(bits);
	break;
	/*
	case 0:
	SPI_uint32(0 | ((uint32_t)1<< (31)) | ((uint32_t)1<< (32-7-(val%10))));
	break;
	case 1:
	SPI_uint32(0 | ((uint32_t)1<< (30)) | ((uint32_t)1<< (32-7-((val%100) /10))));
	break;
	case 2:
	SPI_uint32(0 | ((uint32_t)1<< (29)) | ((uint32_t)1<< (32-7-((val%1000)/100))));
	break;
	case 3:
	SPI_uint32(0 | ((uint32_t)1<< (28)) | ((uint32_t)1<< (32-7-(val % 10000 / 1000))));
	break;
	case 4:
	SPI_uint32(0 | ((uint32_t)1<< (27)) | ((uint32_t)1<< (32-7-(0))));
	break;
	case 5:
	if (dots == 0)
	SPI_uint32(0 | ((uint32_t)1<< (26)) | ((uint32_t)1<< (32-7-(0))));
	else
	SPI_uint32(0 | ((uint32_t)1<< (26)) | ((uint32_t)1<< (32-7-(0)))| ((uint32_t)1<< (32-6-14))|  ((uint32_t)1<< (32-6-12))| ((uint32_t)1<< (32-6-13))|((uint32_t)1<< (32-6-15)));
	break;
*/

	}
	num++;
	
	delayMicroseconds(180);
	SPI_uint32(0);	


	}
//	else
//	SPI_uint32(0);

}


void alarm_fire() {if(alarm < 60) {alarm = 60; tmpvar0=min;}}

void check_alarm(uint8_t idx)
{
uint16_t atime=0;
uint16_t curminutes = (hour * 60) + min;

	switch(idx)
	{
	case 0:

	atime = alarm0;
	if(atime < 5000) return; atime -= 5000;
	if((curminutes == atime) && !((date.weekday == 0) || (date.weekday == 6))) alarm_fire();
	break;


	case 1:

	atime = alarm1;
	if(atime < 5000) return; atime -= 5000;
	if((curminutes == atime) && ((date.weekday == 0) || (date.weekday == 6)))  alarm_fire();
	break;

	default:
	return;
	break;
	}

	


}



int main(void)
{


	DDRB=0xFF;
	SPCR=0x5C;
	SPCR |= _BV(DORD);
  	TIMSK |=  (1 << TOIE0)|(1 << TOIE2); // TOIE1
   	TCCR0 |=  _BV(CS00);
	//TCCR2 |= _BV(CS20) |_BV(CS21) |_BV(CS22);
	TCCR2 |= _BV(CS22) ;

	alarm0 = eeprom_read_alarm(1);
	if ( alarm0 >= 5000)  dots |= _BV(5);

	alarm1 = eeprom_read_alarm(0);
	if ( alarm1 >= 5000)  dots |= _BV(4);

	i2c_init();
	buzzer_init();

	ADCSRA = _BV(ADEN)|_BV(ADPS1);			// Enable the ADC and its interrupt feature
					// and set the ACD clock pre-scalar to clk/128
	ADMUX = _BV(REFS0)|_BV(MUX2)|_BV(MUX1)|_BV(MUX0);			// Select internal 2.56V as Vref, left justify 
					// data registers and select ADC0 as input channel 
 
  	
	//pca2129_set_time(4,52,0);
	//initTinyTune();
    sei(); //enable global interrupts

	ADCSRA |= _BV(ADSC);

    PORTB=0b00000000;
	DDRB &= ~_BV(PB1);
	PORTC |= _BV(PC0)|_BV(PC1)|_BV(PC2)|_BV(PC3);
	//sreg_data =  0b11111110000000000000000000000000;




	while (1) 
	{
		if(mode == MODE_TIME){
		check_alarm(0);
		check_alarm(1);
			
			if(bit_is_clear(PINC, PC0) && bit_is_clear(PINC, PC3))  
			{
				bts(); 
					if (btime > ACT_DELAY * 3) 
					{ 
						mode=MODE_SDATE;
						display = 0xff; // display only 4 digits

					}

			}
			
			else if(bit_is_clear(PINC, PC0)||bit_is_clear(PINC, PC1))  
			{  
					bts(); 
					if (btime > ACT_DELAY * 3)  
					{
					if(bit_is_clear(PINC, PC0))
						{
						mode=MODE_SALARM0; 
						decode_alarm(0); 
						}
						else
						{
						mode=MODE_SALARM1;
						decode_alarm(1); 
						}
					alarm=0; mdelay=0; mpos=0; 
					display = 0b11111100;
					dots |= _BV(7);
					dots &= ~_BV(6);
					}
			}
			else if(bit_is_clear(PINC, PC3) || bit_is_clear(PINC, PC2))  
			{
				bts(); 
					if (btime > ACT_DELAY / 3) 
					{ 
						if(alarm > 0) {alarm=0; mdelay=0; mpos=0; }
					}

			}

				else  btime=0;
			
		}
  
  switch(mode) {
  case MODE_SALARM0:
  process_alarm(0);
  break;
  case MODE_SALARM1:
  process_alarm(1);
  break;
  case MODE_SDATE:
  process_sdate();
  break;
  default:
  break;
  }	
	
	}



}
