/*
 * I2c.h
 *
 *  Created on: 25.02.2013
 *      Author: BARS
 */

#ifndef PCA2129_I2C_H_
#define PCA2129_I2C_H_

#define DELAY 1 // �������� � ���.

#define SDA_PIN           PC4    //����� SDA
#define SCL_PIN           PC5    //����� SCL

#define SDA_PORT_READ     PINC   //���� �����
#define SCL_PORT_READ     PINC   //���� �������� ����� SCL

#define SDA_PORT_DIR      DDRC   //���� �����������
#define SCL_PORT_DIR      DDRC   //���� �����������
#define SDA_PORT          PORTC  //���� ������
#define SCL_PORT          PORTC  //���� ������

void i2c_init();      // ������������� i2c
void i2c_start();     // ������ i2c
void i2c_stop();      // ��������� i2c
signed char  i2c_write(unsigned char  byte);  // ������
unsigned char  i2c_read(unsigned char  ack);  // ������

#endif /* I2C_H_ */
