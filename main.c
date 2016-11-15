#include "xc.h"
#include <htc.h>
#define _XTAL_FREQ 8000000
#pragma config FOSC     = INTOSC
#pragma config WDTE     = OFF
#pragma config PWRTE    = OFF
#pragma config MCLRE    = ON
#pragma config CP       = OFF
#pragma config BOREN    = ON
#pragma config CLKOUTEN = OFF
#pragma config IESO     = OFF
#pragma config FCMEN    = OFF
#pragma config WRT      = OFF
#pragma config PLLEN    = OFF
#pragma config STVREN   = ON
#pragma config BORV     = HI
#pragma config LVP      = OFF

int AckCheck = 0;

void idle_check(char mask) {
	while ((SSPCON2 & 0x1F) | (SSPSTAT & mask));
}

void interrupt InterFunction( void ) {
	if (SSP1IF == 1) {
		if (AckCheck == 1) AckCheck = 0;
		SSP1IF = 0;
	}
	if (BCL1IF == 1) {
		if (AckCheck == 1) AckCheck = 0;
		BCL1IF = 0;
	}
}

int start(int rw) {
	idle_check(0x5);
	SSPCON2bits.SEN = 1;
	idle_check(0x5);
	AckCheck = 1;
	SSPBUF = 0x7c + rw;
	__delay_us(27);
	while (AckCheck);
	return SSPCON2bits.ACKSTAT;
}

int send(char dt) {
	idle_check(0x5);
	AckCheck = 1;
	SSPBUF = dt;
	__delay_us(27);
	while (AckCheck);
	return SSPCON2bits.ACKSTAT;
}

void stop() {
	idle_check(0x5);
	SSPCON2bits.PEN = 1;
}

void command(unsigned char c) {
	start(0);
	send(0b00000000);
	send(c);
	stop();
}

void write(unsigned char c) {
	start(0);
	send(0b01000000);
	send(c);
	stop();
}

void main(void) {
	OSCCON     = 0b01110010; // 8MHz
	ANSELA     = 0b00000000;
	ANSELC     = 0b00000000;
	TRISA      = 0b00000000;
	TRISC      = 0b00000011;
	WPUC       = 0b00000011;
	OPTION_REG = 0b00000000;
	PORTA      = 0;
	PORTC      = 0;
	RC0PPS     = 0b10000;
	RC1PPS     = 0b10001;

	__delay_ms(200);
	SSPSTAT = 0b10000000;
	SSPCON1 = 0b00101000;
	SSPADD = 0x13;
	SSP1IE = 1;
	BCL1IE = 1;
	PEIE = 1;
	GIE = 1;
	SSP1IF = 0;
	BCL1IF = 0;

	__delay_ms(100);
	command(0x38);
	__delay_us(27);
	command(0x39);
	__delay_us(27);
	command(0x14);
	__delay_us(27);
	command(0x70);
	__delay_us(27);
	command(0x56);
	__delay_us(27);
	command(0x6c);
	__delay_ms(200);
	command(0x38);
	__delay_us(27);
	command(0x0c);
	__delay_us(27);
	command(0x06);
	__delay_ms(1);
	command(0x01);
	__delay_us(1100);
	command(0x02);
	__delay_us(1100);

	unsigned int i = 0;
	while (1) {
		int j = i++;
		command(0x80);
		write(j++);
		write(j++);
		write(j++);
		write(j++);
		write(j++);
		write(j++);
		write(j++);
		write(j++);
		command(0x80 | 0x40);
		__delay_ms(1);
		write(j++);
		write(j++);
		write(j++);
		write(j++);
		write(j++);
		write(j++);
		write(j++);
		write(j++);
		__delay_ms(100);
	}

	command(0x80);
	write('I');
	write(' ');
	write('l');
	write('o');
	write('v');
	write('e');
	write(' ');
	write(' ');
	command(0x80 | 0x40);
	__delay_ms(1);
	write('A');
	write('r');
	write('d');
	write('u');
	write('i');
	write('n');
	write('o');
	write('.');
	while(1) {
	}

	return;
}
