/*
 * pca2129_i2c.h
 *
 *  Created on: 25.02.2013
 *      Author: BARS
 */

#ifndef PCA2129_I2C_H_
#define PCA2129_I2C_H_

void pca2129_init(unsigned char t_meas, unsigned char out); // инициализация
void pca2129_set_ds(void);
void pca2129_set_time(unsigned char hour,unsigned char min,unsigned char sec); // установка времени
void pca2129_get_time(unsigned char *hour,unsigned char *min,unsigned char *sec); // чтение времени
void pca2129_set_date(unsigned char day, unsigned char weekday, unsigned char month, unsigned char year); // установка даты
void pca2129_get_date(unsigned char *day, unsigned char *month, unsigned char *year, unsigned char *weekday); // чтение даты
void pca2129_set_alarm(unsigned char hour, unsigned char min, unsigned char sec, unsigned char day, unsigned char start); // установка будильника
unsigned char pca2129_get_alarm(); // чтение состояния будильника
unsigned char low_bat(); // чтение состояния батареи
void WDT(unsigned char en, unsigned char clock, unsigned char period); // настройка сторожевого таймера

#endif /* PCA2129_I2C_H_ */
