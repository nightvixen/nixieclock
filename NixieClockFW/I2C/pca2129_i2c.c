/*
 * pca2129_i2c.c
 *
 *  Created on: 25.02.2013
 *      Author: BARS
 */
#include <avr/interrupt.h>
#include "../i2cmaster.h"
#include "pca2129_i2c.h"

unsigned char bcd (unsigned char data)
{
	unsigned char bc;

	bc=((((data&(1<<6))|(data&(1<<5))|(data&(1<<4)))*0x0A)>>4)+((data&(1<<3))|(data&(1<<2))|(data&(1<<1))|(data&0x01));

	return bc;
}

unsigned char bin(unsigned char dec){

	char bcd;

	char n, dig, num, count;

	num = dec;
	count = 0;
	bcd = 0;
	for (n=0; n<4; n++) {
		dig = num%10;
		num = num/10;
		bcd = (dig<<count)|bcd;
		count += 4;
	}

	return bcd;
}

/*
 * Имя                   :  pca2129_init
 * Описание              :  Производит инициализацию i2c МК и PCA2129
 * Аргументы             :
 //
 // t_meas - интервал измерения температуры для температурной компенсации
 // 0 - 4 мин.
 // 1 - 2 мин.
 // 2 - 1 мин.
 // 3 - 30 сек.
 //
 // out - частота тактового сигнала на выводе CLK_OUT
 // 0 - 32.768 KHz.
 // 1 - 16.384 KHz.
 // 2 - 8.192 KHz.
 // 3 - 4.096 KHz.
 // 4 - 2.048 KHz.
 // 5 - 1.024 KHz.
 // 6 - 1 Hz.
 // 7 - тактовый сигнал отключен.
 //
 * Возвращаемое значение :  Нет
 */
void pca2129_init(unsigned char t_meas, unsigned char out){

	i2c_init(); // инициализация i2c

/*	unsigned char r = 0;

	r |= (t_meas<<6) | (out<<0); // запись значений для отправки в pca2129

	i2c_start();     // запуск i2c
	i2c_write(0xA2); // передача адреса устройства, режим записи
	i2c_write(0x0F); // передача адреса памяти
	i2c_write(r);    // запись значения
	i2c_stop();      // остановка i2c
	*/
}

/*
 * Имя                   :  pca2129_set_time
 * Описание              :  Установка времени в PCA2129
 * Аргументы             :  hour - часы
 * 							min - минуты
 * 							sec - секунды
 * Возвращаемое значение :  Нет
 */
void pca2129_set_time(unsigned char hour,unsigned char min,unsigned char sec){

	i2c_start(0xA2);  // передача адреса устройства, режим записи
	i2c_write(3);	 // передача адреса памяти
	i2c_write(bin(sec));  // запись секунд
	i2c_write(bin(min));  // запись минут
	i2c_write(bin(hour)); // запись часов
	i2c_stop();		 // остановка i2c


}

void pca2129_set_ds(){

	i2c_start(0xA2);  // передача адреса устройства, режим записи
	i2c_write(2);	 // Control_3
	i2c_write(0b10000000);  // direct switching, battery low disabled

	i2c_stop();		 // остановка i2c


}



/*
 * Имя                   :  pca2129_get_time
 * Описание              :  Чтение времени из PCA2129
 * Аргументы             :  Нет
 * Возвращаемое значение :  hour - часы
 * 							min - минуты
 * 							sec - секунды
 */
void pca2129_get_time(unsigned char *hour,unsigned char *min,unsigned char *sec){
	i2c_start_wait(0xA2);  // передача адреса устройства, режим записи	
	i2c_write(3);	  // передача адреса памяти
	i2c_stop();
	i2c_start_wait(0xA3);		   // передача адреса устройства, режим чтения
	*sec = bcd(i2c_readAck());   // чтение секунд
	*min = bcd(i2c_readAck());   // чтение минут
	*hour = bcd(i2c_readNak());  // чтение часов
	i2c_stop();			       // остановка i2c
}


/*
 * Имя                   :  pca2129_set_date
 * Описание              :  Установка даты в PCA2129
 * Аргументы             :  day - день
 * 							month - месяц
 * 							year - год
 * 							weekday - номер дня недели
 * Возвращаемое значение :  Нет
 */
void pca2129_set_date(unsigned char day, unsigned char weekday, unsigned char month, unsigned char year){

	i2c_start_wait(0xA2); // передача адреса устройства, режим записи
	i2c_write(6);	 // передача адреса памяти
	i2c_write(bin(day));     // запись дня
	i2c_write(bin(weekday)); // запись номера дня недели
	i2c_write(bin(month));   // запись месяца
	i2c_write(bin(year));    // запись года
	i2c_stop();		 // остановка i2c

/*

	i2c_start_wait(0xA2); // передача адреса устройства, режим записи
	i2c_write(0x06);	 // передача адреса памяти
	i2c_write(bin(day));     // запись дня
	i2c_stop();

	i2c_start_wait(0xA2); // передача адреса устройства, режим записи
	i2c_write(0x08);	 // передача адреса памяти
	i2c_write(bin(month));   // запись месяца
	i2c_write(bin(year));    // запись года
	i2c_stop();		 // остановка i2c
*/
}


/*
 * Имя                   :  pca2129_get_date
 * Описание              :  Чтение даты из PCA2129
 * Аргументы             :  Нет
 * Возвращаемое значение :  day - день
 * 							month - месяц
 * 							year - год
 * 							weekday - номер дня недели
 */
void pca2129_get_date(unsigned char *day, unsigned char *month, unsigned char *year, unsigned char *weekday){

	i2c_start_wait(0xA2); // передача адреса устройства, режим записи
	i2c_write(6);	 // передача адреса памяти
	i2c_stop();		 // остановка i2c

	i2c_start_wait(0xA3); // передача адреса устройства, режим чтения
	*day= bcd(i2c_readAck());      // чтение дня
	*weekday = bcd(i2c_readAck()); // чтение номера дня недели
	*month = bcd(i2c_readAck());   // чтение месяца
	*year = bcd(i2c_readNak());    // чтение года
	i2c_stop();      // остановка i2c
}
#if 0
/*
 * Имя                   :  pca2129_set_alarm
 * Описание              :  Установка будильника в PCA2129
 * Аргументы             :  hour - час
 * 							min - минута
 * 							sec - секунда
 * 							day - день
 * 							start - запуск будильника: 1-включен, 0-выключен.
 * Возвращаемое значение :  Нет
 */
/*
void pca2129_set_alarm(unsigned char hour, unsigned char min, unsigned char sec, unsigned char day, unsigned char start){

	unsigned char r1 = 0, r2 = 0, r3 = 0, r4 = 0;

	if(start == 1){
		r1 |= (0<<7) | (bin(sec)<<0);
		r2 |= (0<<7) | (bin(min)<<0);
		r3 |= (0<<7) | (bin(hour)<<0);
		r4 |= (0<<7) | (bin(day)<<0);
	}else{
		r1 |= (1<<7) | (bin(sec)<<0);
		r2 |= (1<<7) | (bin(min)<<0);
		r3 |= (1<<7) | (bin(hour)<<0);
		r4 |= (1<<7) | (bin(day)<<0);
	}

	i2c_start();     // запуск i2c
	i2c_write(0xA2); // передача адреса устройства, режим чтения
	i2c_write(0x0A); // передача адреса памяти
	i2c_write(r1);   // запись секунд
	i2c_write(r2);   // запись минут
	i2c_write(r3);   // запись часов
	i2c_write(r4);   // запись дней
	i2c_stop();      // остановка i2c
}
*/
/*
 * Имя                   :  pca2129_get_alarm
 * Описание              :  Чтение состояния будильника из PCA2129
 * Аргументы             :  Нет
 * Возвращаемое значение :  1 - если сработал, 0 - если не сработал
 */
unsigned char pca2129_get_alarm(){
	unsigned char alarm = 0, ret = 0;

	i2c_start();     // запуск i2c
	i2c_write(0xA2); // передача адреса устройства, режим записи
	i2c_write(1);	 // передача адреса памяти
	i2c_stop();		 // остановка i2c

	i2c_start();     // запуск i2c
	i2c_write(0xA3); // передача адреса устройства, режим чтения
	alarm = i2c_read(0); // чтение регистра данных
	i2c_read(1); // на всякий случай, для корректного завершения чтения
	i2c_stop();  // остановка i2c

	if ((alarm & (1<<4))){ // если в 4 бите 1, значит сработал
		ret = 1;
		alarm &= (0<<4); // обнуляем 4 бит

		i2c_start();      // запуск i2c
		i2c_write(0xA2);  // передача адреса устройства, режим записи
		i2c_write(1);	  // передача адреса памяти
		i2c_write(alarm); // запись значения в регистр
		i2c_stop();		  // остановка i2c
	}else{
		ret = 0;
	}

	return ret;

}

/*
 * Имя                   :  pca2129_low_bat
 * Описание              :  Чтение состояния резервной батареи из PCA2129
 * Аргументы             :  Нет
 * Возвращаемое значение :  1 - если разряжена, 0 - если не разряжена
 */
unsigned char low_bat(){
	char low = 0, ret = 0;

	i2c_start();     // запуск i2c
	i2c_write(0xA2); // передача адреса устройства, режим записи
	i2c_write(2);	 // передача адреса памяти
	i2c_stop();		 // остановка i2c

	i2c_start();     // запуск i2c
	i2c_write(0xA3); // передача адреса устройства, режим чтения
	low = i2c_read(0); // чтение регистра данных
	i2c_read(1); // на всякий случай, для корректного завершения чтения
	i2c_stop();  // остановка i2c

	if((low & (1<<2))) ret = 1; else ret = 0; // если во 2 бите 1, значит разряжена

	return ret;
}

/*
 * Имя                   :  WDT
 * Описание              :  Настройка сторожевого таймера PCA2129
 * Аргументы             :  en - включение таймера: 1-включен, 0-выключен
 * 							clock - настройка частоты счетчика
 * 							0 - 4096 Hz.
 * 							1 - 64 Hz.
 * 							2 - 1 Hz.
 * 							3 - 1/60 Hz
 * 							peiod - значение до которого будет идти счет. от 0 до 255
 * 							Время расчитывается по формуле: period/clock(Hz.)
 * 							При срабатывании выдается высокий логический уровень на вывод INT
 * Возвращаемое значение :  Нет
 */
void WDT(unsigned char en, unsigned char clock, unsigned char period){

	unsigned char set = 0;

	set |= (en<<7)|(clock<<0);

	i2c_start();     // запуск i2c
	i2c_write(0xA2); // передача адреса устройства, режим записи
	i2c_write(0);	 // передача адреса памяти
	i2c_write(1);	 // активизация прерываний
	i2c_stop();		 // остановка i2c

	i2c_start();     // запуск i2c
	i2c_write(0xA2); // передача адреса устройства, режим записи
	i2c_write(10);	 // передача адреса памяти

	i2c_write(set);    // запись значений
	i2c_write(period); // запись значений
	i2c_stop();		   // остановка i2c
}

#endif
