/*
 * I2c.h
 *
 *  Created on: 25.02.2013
 *      Author: BARS
 */

#ifndef PCA2129_I2C_H_
#define PCA2129_I2C_H_

#define DELAY 1 // задержка в мкс.

#define SDA_PIN           PC4    //линия SDA
#define SCL_PIN           PC5    //линия SCL

#define SDA_PORT_READ     PINC   //порт входа
#define SCL_PORT_READ     PINC   //порт проверки линии SCL

#define SDA_PORT_DIR      DDRC   //порт направления
#define SCL_PORT_DIR      DDRC   //порт направления
#define SDA_PORT          PORTC  //порт выхода
#define SCL_PORT          PORTC  //порт выхода

void i2c_init();      // инициализация i2c
void i2c_start();     // запуск i2c
void i2c_stop();      // остановка i2c
signed char  i2c_write(unsigned char  byte);  // запись
unsigned char  i2c_read(unsigned char  ack);  // чтение

#endif /* I2C_H_ */
