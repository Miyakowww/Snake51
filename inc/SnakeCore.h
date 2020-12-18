#ifndef SNAKECORE
#define SNAKECORE

extern char map[8][8];
extern unsigned char HeadPos;
extern unsigned char Direction;

extern void InitLevel(unsigned char id);
extern unsigned char GoOneStep();

#define D_UP 0x01
#define D_DOWN 0x07
#define D_LEFT 0x70
#define D_RIGHT 0x10

#endif