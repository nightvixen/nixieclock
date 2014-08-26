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
 * ���                   :  pca2129_init
 * ��������              :  ���������� ������������� i2c �� � PCA2129
 * ���������             :
 //
 // t_meas - �������� ��������� ����������� ��� ������������� �����������
 // 0 - 4 ���.
 // 1 - 2 ���.
 // 2 - 1 ���.
 // 3 - 30 ���.
 //
 // out - ������� ��������� ������� �� ������ CLK_OUT
 // 0 - 32.768 KHz.
 // 1 - 16.384 KHz.
 // 2 - 8.192 KHz.
 // 3 - 4.096 KHz.
 // 4 - 2.048 KHz.
 // 5 - 1.024 KHz.
 // 6 - 1 Hz.
 // 7 - �������� ������ ��������.
 //
 * ������������ �������� :  ���
 */
void pca2129_init(unsigned char t_meas, unsigned char out){

	i2c_init(); // ������������� i2c

/*	unsigned char r = 0;

	r |= (t_meas<<6) | (out<<0); // ������ �������� ��� �������� � pca2129

	i2c_start();     // ������ i2c
	i2c_write(0xA2); // �������� ������ ����������, ����� ������
	i2c_write(0x0F); // �������� ������ ������
	i2c_write(r);    // ������ ��������
	i2c_stop();      // ��������� i2c
	*/
}

/*
 * ���                   :  pca2129_set_time
 * ��������              :  ��������� ������� � PCA2129
 * ���������             :  hour - ����
 * 							min - ������
 * 							sec - �������
 * ������������ �������� :  ���
 */
void pca2129_set_time(unsigned char hour,unsigned char min,unsigned char sec){

	i2c_start(0xA2);  // �������� ������ ����������, ����� ������
	i2c_write(3);	 // �������� ������ ������
	i2c_write(bin(sec));  // ������ ������
	i2c_write(bin(min));  // ������ �����
	i2c_write(bin(hour)); // ������ �����
	i2c_stop();		 // ��������� i2c


}

void pca2129_set_ds(){

	i2c_start(0xA2);  // �������� ������ ����������, ����� ������
	i2c_write(2);	 // Control_3
	i2c_write(0b10000000);  // direct switching, battery low disabled

	i2c_stop();		 // ��������� i2c


}



/*
 * ���                   :  pca2129_get_time
 * ��������              :  ������ ������� �� PCA2129
 * ���������             :  ���
 * ������������ �������� :  hour - ����
 * 							min - ������
 * 							sec - �������
 */
void pca2129_get_time(unsigned char *hour,unsigned char *min,unsigned char *sec){
	i2c_start_wait(0xA2);  // �������� ������ ����������, ����� ������	
	i2c_write(3);	  // �������� ������ ������
	i2c_stop();
	i2c_start_wait(0xA3);		   // �������� ������ ����������, ����� ������
	*sec = bcd(i2c_readAck());   // ������ ������
	*min = bcd(i2c_readAck());   // ������ �����
	*hour = bcd(i2c_readNak());  // ������ �����
	i2c_stop();			       // ��������� i2c
}


/*
 * ���                   :  pca2129_set_date
 * ��������              :  ��������� ���� � PCA2129
 * ���������             :  day - ����
 * 							month - �����
 * 							year - ���
 * 							weekday - ����� ��� ������
 * ������������ �������� :  ���
 */
void pca2129_set_date(unsigned char day, unsigned char weekday, unsigned char month, unsigned char year){

	i2c_start_wait(0xA2); // �������� ������ ����������, ����� ������
	i2c_write(6);	 // �������� ������ ������
	i2c_write(bin(day));     // ������ ���
	i2c_write(bin(weekday)); // ������ ������ ��� ������
	i2c_write(bin(month));   // ������ ������
	i2c_write(bin(year));    // ������ ����
	i2c_stop();		 // ��������� i2c

/*

	i2c_start_wait(0xA2); // �������� ������ ����������, ����� ������
	i2c_write(0x06);	 // �������� ������ ������
	i2c_write(bin(day));     // ������ ���
	i2c_stop();

	i2c_start_wait(0xA2); // �������� ������ ����������, ����� ������
	i2c_write(0x08);	 // �������� ������ ������
	i2c_write(bin(month));   // ������ ������
	i2c_write(bin(year));    // ������ ����
	i2c_stop();		 // ��������� i2c
*/
}


/*
 * ���                   :  pca2129_get_date
 * ��������              :  ������ ���� �� PCA2129
 * ���������             :  ���
 * ������������ �������� :  day - ����
 * 							month - �����
 * 							year - ���
 * 							weekday - ����� ��� ������
 */
void pca2129_get_date(unsigned char *day, unsigned char *month, unsigned char *year, unsigned char *weekday){

	i2c_start_wait(0xA2); // �������� ������ ����������, ����� ������
	i2c_write(6);	 // �������� ������ ������
	i2c_stop();		 // ��������� i2c

	i2c_start_wait(0xA3); // �������� ������ ����������, ����� ������
	*day= bcd(i2c_readAck());      // ������ ���
	*weekday = bcd(i2c_readAck()); // ������ ������ ��� ������
	*month = bcd(i2c_readAck());   // ������ ������
	*year = bcd(i2c_readNak());    // ������ ����
	i2c_stop();      // ��������� i2c
}
#if 0
/*
 * ���                   :  pca2129_set_alarm
 * ��������              :  ��������� ���������� � PCA2129
 * ���������             :  hour - ���
 * 							min - ������
 * 							sec - �������
 * 							day - ����
 * 							start - ������ ����������: 1-�������, 0-��������.
 * ������������ �������� :  ���
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

	i2c_start();     // ������ i2c
	i2c_write(0xA2); // �������� ������ ����������, ����� ������
	i2c_write(0x0A); // �������� ������ ������
	i2c_write(r1);   // ������ ������
	i2c_write(r2);   // ������ �����
	i2c_write(r3);   // ������ �����
	i2c_write(r4);   // ������ ����
	i2c_stop();      // ��������� i2c
}
*/
/*
 * ���                   :  pca2129_get_alarm
 * ��������              :  ������ ��������� ���������� �� PCA2129
 * ���������             :  ���
 * ������������ �������� :  1 - ���� ��������, 0 - ���� �� ��������
 */
unsigned char pca2129_get_alarm(){
	unsigned char alarm = 0, ret = 0;

	i2c_start();     // ������ i2c
	i2c_write(0xA2); // �������� ������ ����������, ����� ������
	i2c_write(1);	 // �������� ������ ������
	i2c_stop();		 // ��������� i2c

	i2c_start();     // ������ i2c
	i2c_write(0xA3); // �������� ������ ����������, ����� ������
	alarm = i2c_read(0); // ������ �������� ������
	i2c_read(1); // �� ������ ������, ��� ����������� ���������� ������
	i2c_stop();  // ��������� i2c

	if ((alarm & (1<<4))){ // ���� � 4 ���� 1, ������ ��������
		ret = 1;
		alarm &= (0<<4); // �������� 4 ���

		i2c_start();      // ������ i2c
		i2c_write(0xA2);  // �������� ������ ����������, ����� ������
		i2c_write(1);	  // �������� ������ ������
		i2c_write(alarm); // ������ �������� � �������
		i2c_stop();		  // ��������� i2c
	}else{
		ret = 0;
	}

	return ret;

}

/*
 * ���                   :  pca2129_low_bat
 * ��������              :  ������ ��������� ��������� ������� �� PCA2129
 * ���������             :  ���
 * ������������ �������� :  1 - ���� ���������, 0 - ���� �� ���������
 */
unsigned char low_bat(){
	char low = 0, ret = 0;

	i2c_start();     // ������ i2c
	i2c_write(0xA2); // �������� ������ ����������, ����� ������
	i2c_write(2);	 // �������� ������ ������
	i2c_stop();		 // ��������� i2c

	i2c_start();     // ������ i2c
	i2c_write(0xA3); // �������� ������ ����������, ����� ������
	low = i2c_read(0); // ������ �������� ������
	i2c_read(1); // �� ������ ������, ��� ����������� ���������� ������
	i2c_stop();  // ��������� i2c

	if((low & (1<<2))) ret = 1; else ret = 0; // ���� �� 2 ���� 1, ������ ���������

	return ret;
}

/*
 * ���                   :  WDT
 * ��������              :  ��������� ����������� ������� PCA2129
 * ���������             :  en - ��������� �������: 1-�������, 0-��������
 * 							clock - ��������� ������� ��������
 * 							0 - 4096 Hz.
 * 							1 - 64 Hz.
 * 							2 - 1 Hz.
 * 							3 - 1/60 Hz
 * 							peiod - �������� �� �������� ����� ���� ����. �� 0 �� 255
 * 							����� ������������� �� �������: period/clock(Hz.)
 * 							��� ������������ �������� ������� ���������� ������� �� ����� INT
 * ������������ �������� :  ���
 */
void WDT(unsigned char en, unsigned char clock, unsigned char period){

	unsigned char set = 0;

	set |= (en<<7)|(clock<<0);

	i2c_start();     // ������ i2c
	i2c_write(0xA2); // �������� ������ ����������, ����� ������
	i2c_write(0);	 // �������� ������ ������
	i2c_write(1);	 // ����������� ����������
	i2c_stop();		 // ��������� i2c

	i2c_start();     // ������ i2c
	i2c_write(0xA2); // �������� ������ ����������, ����� ������
	i2c_write(10);	 // �������� ������ ������

	i2c_write(set);    // ������ ��������
	i2c_write(period); // ������ ��������
	i2c_stop();		   // ��������� i2c
}

#endif
