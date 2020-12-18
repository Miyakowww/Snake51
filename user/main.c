#include <reg52.h>
#include <stdlib.h>
#include <intrins.h>
#include "Typedef.h"
#include "SnakeCore.h"
#include "LCDLib.h"

/* 0b 0    0         000000
 *    Fog  OnSelect  SelectedMapID
 */
uchar appStat;
uchar gstatus;
uint rseed = 0;

sbit SER = P3^4;
sbit SRCLK = P3^6;
sbit _RCLK = P3^5;

sbit KL = P1^3;
sbit KR = P1^1;
sbit KU = P1^7;
sbit KD = P1^5;
sbit KOK = P1^0;

void sleep();

void main() {
	uchar i;

	EA = 1;
	TMOD = 0x11;

	InitLCD();

	while (1) {
		// Clean LED
		SER = 0;
		P1 = 0xAB;
		for (i = 0; i < 8; i++) {
			SRCLK = 1;
			_nop_();   
			_nop_();
			SRCLK = 0;
			_nop_();  
			_nop_();
		}

		// Menu
		while (1) {
			WriteCMD(0x01);
			if (appStat & 0x40) {
				WriteStr(0, "Select Map");
				switch (appStat & 0x3F) {
				case 0:
					WriteStr(24, "Normal >");
					break;
				case 1:
					WriteStr(26, "Room >");
					break;
				case 2:
					WriteStr(25, "Cross >");
					break;
				case 3:
					WriteStr(29, "T >");
					break;
				}
			}
			else {
				WriteStr(0, "Select Mode");
				if (appStat & 0x80) {
					WriteStr(22, "Fog Mode >");
				}
				else {
					WriteStr(18, "Classic Mode >");
				}
			}	   	   
			while ((P1 & 0xAB) == 0xAB) rseed++;
			if (!KR || !KOK) {
				if (appStat & 0x40) {
					srand(rseed);
					InitLevel(appStat & 0x3F);
					break;
				}
				else {
					appStat |= 0x40;
				}
			}
			else if (!KL) {
				appStat &= 0xBF;
			}
			else if (!KU) {
				if (appStat & 0x40) {
					appStat = (appStat & 0x3F) > 0 ? appStat - 1 : appStat;
				}
				else {
					appStat &= 0x7F;
				}
			}
			else if (!KD) {
				if (appStat & 0x40) {
					appStat = (appStat & 0x3F) < 3 ? appStat + 1 : appStat;
				}
				else {			   
					appStat |= 0x80;  
				}
			}	   
			sleep();
			while ((P1 & 0xAB) != 0xAB) rseed++;
			sleep(); 
		}

		// Gameplay
		WriteCMD(0x01);
		gstatus = 0;
		TH0 = 0x4C;
		TL0 = 0x00;
		TH1 = 0xFE;
		TL1 = 0x00;
		ET0 = ET1 = 1;
		TR0 = TR1 = 1;
		while (!gstatus);
		TR0 = TR1 = 0;
		ET0 = ET1 = 0;

		// Settlement
		WriteCMD(0x01);
		WriteStr(0, "Score:");
		WriteNum(7, gstatus - 1);
		while (KOK);
		sleep();
		while (!KOK);
		sleep();
	}
}

void sleep() {
	int i;
	for (i = 0; i < 200; i++);
}

int walkcnt = 0;

void Walk() interrupt 1 {
	int dt;
	TH0 = 0x4C;
	TL0 = 0x00;

	walkcnt++;
	if (walkcnt >= 10) {
		walkcnt = 0;
		gstatus = GoOneStep();
	}

	dt = !KL ? D_LEFT : (!KR ? D_RIGHT : (!KU ? D_UP : (!KD ? D_DOWN : 0)));
	if (dt && !((dt + Direction) & 0x88)) {
		Direction = dt;
	}
}

uchar GScnt = 0xFF;
uchar ScanPos = 0xFF;

void DispLED() interrupt 3 {
	uchar i;
	uchar buff = 0;
	char t, tp, tmp;

	TH1 = 0xFE;
	TL1 = 0x00;

	P0 = 0xFF;

	SRCLK = 0;
	_RCLK = 0;
	ScanPos = (ScanPos + 1) & 0x07;
	if (!ScanPos) {
		GScnt = (GScnt + 1) & 0x0F; // 16
		SER = 1;
		SRCLK = 1;
		_nop_();   
		_nop_();
		SRCLK = 0;
	}
	else {
		SER = 0;
		SRCLK = 1;
		_nop_();  
		_nop_();
		SRCLK = 0;
	}
	_RCLK = 1;
	_nop_();	   
	_nop_();
	_RCLK = 0;

	if (appStat & 0x80) {
		// Fog
		for (i = 0; i < 8; i++) {
			tp = HeadPos >> 4;
			t = 7 - i;
			t = (tp > t) ? tp - t : t - tp;
			t = (t & 0x40) ? 8 - t : t;
			tmp = 4 - t;
			tp = HeadPos & 0x07;
			t = ScanPos;
			t = (tp > t) ? tp - t : t - tp;
			t = (t & 0x40) ? 8 - t : t;
			tmp = tmp - t;
			//tmp = -tmp * tmp / 16.0f + 1;
			t = map[7 - i][ScanPos];
			if (!t);
			else if (t > 0) { // Snake
				if (GScnt < 12 * tmp) {
					buff |= 1 << i;
				}
			}
			else if (t == -2) { // Wall
				if (GScnt < 16 * tmp) {
					buff |= 1 << i;
				}
			}
			else if (t == -1) { // Food
				if (GScnt < 8 * tmp) {
					buff |= 1 << i;
				}
			}
		}
	}
	else {
		// Normal
		for (i = 0; i < 8; i++) {
			tmp = map[7 - i][ScanPos];
			if (!tmp);
			else if (tmp > 0) { // Snake
				if (GScnt % 2) {
					buff |= 1 << i;
				}
			}
			else if (tmp == -2) { // Wall
				buff |= 1 << i;
			}
			else if (tmp == -1) { // Food
				if (GScnt % 4 == 0) {
					buff |= 1 << i;
				}
			}
		}
	}

	P0 = ~buff;
}