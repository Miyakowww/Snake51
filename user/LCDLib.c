#include <reg52.h>	
#include "LCDLib.h"
#include "Typedef.h"

sbit LCDRS = P2^6;
sbit LCDRW = P2^5;
sbit LCDEN = P2^7;

void Delayms(uint c)
{
    uchar a, b;
	for (; c > 0; c--)
		for (b = 199; b > 0; b--)
		  	for (a = 1; a > 0; a--);
}

void InitLCD() {
	WriteCMD(0x38);	 
	WriteCMD(0x0C);
	WriteCMD(0x06);
	WriteCMD(0x01);
}
void WriteCMD(uchar cmd) {	 
	LCDEN = 0;
	LCDRS = 0;	
	LCDRW = 0;
	P0 = cmd; 
	Delayms(1);
	LCDEN = 1;	 
	Delayms(5);
	LCDEN = 0;
}
void WriteData(uchar dat) {
	LCDEN = 0;
	LCDRS = 1;	 
	LCDRW = 0;
	P0 = dat;
	Delayms(1);
	LCDEN = 1;	
	Delayms(5);
	LCDEN = 0;
}
void WriteStr(uchar pos, uchar* str) {
	WriteCMD((pos > 15 ? 0x30 + pos : pos) | 0x80);
	while (*str) {
		WriteData(*str++);
	}
}
void WriteNum(uchar pos, uint num) {
	char str[7];
	int ptr = 5;
	str[6] = 0;
	while (num) {
		str[--ptr] = num % 10 + 0x30;
		num /= 10;
	}
	WriteStr(pos, str + ptr);
}
