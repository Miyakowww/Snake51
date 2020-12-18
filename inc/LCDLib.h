#ifndef LCDLIB
#define LCDLIB

extern void InitLCD();
extern void WriteCMD(unsigned char cmd);
extern void WriteData(unsigned char dat);
extern void WriteStr(unsigned char pos, unsigned char* str);
extern void WriteNum(unsigned char pos, unsigned int num);

#endif