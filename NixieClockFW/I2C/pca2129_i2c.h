/*
 * pca2129_i2c.h
 *
 *  Created on: 25.02.2013
 *      Author: BARS
 */

#ifndef PCA2129_I2C_H_
#define PCA2129_I2C_H_

void pca2129_init(unsigned char t_meas, unsigned char out); // �������������
void pca2129_set_ds(void);
void pca2129_set_time(unsigned char hour,unsigned char min,unsigned char sec); // ��������� �������
void pca2129_get_time(unsigned char *hour,unsigned char *min,unsigned char *sec); // ������ �������
void pca2129_set_date(unsigned char day, unsigned char weekday, unsigned char month, unsigned char year); // ��������� ����
void pca2129_get_date(unsigned char *day, unsigned char *month, unsigned char *year, unsigned char *weekday); // ������ ����
void pca2129_set_alarm(unsigned char hour, unsigned char min, unsigned char sec, unsigned char day, unsigned char start); // ��������� ����������
unsigned char pca2129_get_alarm(); // ������ ��������� ����������
unsigned char low_bat(); // ������ ��������� �������
void WDT(unsigned char en, unsigned char clock, unsigned char period); // ��������� ����������� �������

#endif /* PCA2129_I2C_H_ */
