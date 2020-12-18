#include <stdlib.h>
#include "SnakeCore.h"
#include "Typedef.h"

/* Coordinate Definition
 * Y
 * ^
 * |
 * 0---> X
 *
 * Map Mark
 * 0 => Empty
 * -1 => Food
 * -2 => Wall
 * Greater than 0 => Health
 */

char map[8][8];
uchar HeadPos;
uchar Direction;
uchar length;
uchar dlength;
uchar code LevelInfo[] = { // HeadPos[2], Dir, Len, Wall[8]
	0x13, D_RIGHT, 3, 0,0,0,0,0,0,0,0,                          // Normal
	0x13, D_RIGHT, 3, 0xFF,0x81,0x81,0x81,0x81,0x81,0x81,0xFF,  // Room
	0x13, D_RIGHT, 3, 0x04,0x04,0xFF,0x04,0x04,0x04,0x04,0x04,  // Cross
	0x13, D_RIGHT, 3, 0x00,0x00,0x00,0x00,0xFF,0x08,0x08,0x08,  // T
};

void InitLevel(uchar id) {
	char i, j, fx, fy;

	// Init Map
	for (j = 7; j >= 0; j--)
		for (i = 0; i < 8; i++)
			map[i][j] = (*(LevelInfo + id * 11 + 3 + j) & (0x80 >> i)) ? -2 : 0;

	// Set Head
	HeadPos = LevelInfo[id * 11];
	map[HeadPos >> 4][HeadPos & 0x07] = length = LevelInfo[id * 11 + 2];
	dlength = length - 1;

	// Set Direction
	Direction = LevelInfo[id * 11 + 1];	

	// Set Food
	do {
		fx = rand() & 0x07;
		fy = rand() & 0x07;
	} while (i++ < 20 && map[fx][fy]);
	if (map[fx][fy]) {
		for (i = 0; i < 8; i++) {
			for (j = 0; j < 8; j++) {
				if (!map[i][j]) {
					fx = i;
					fy = j;
					goto out;
				}
			}
		}
	}
out:
	map[fx][fy] = -1;
}

uchar GoOneStep() {
	uchar tx, ty, fx, fy, i = 0, j;
	HeadPos = (HeadPos + Direction) & 0x77;
	tx = HeadPos >> 4;
	ty = HeadPos & 0x07;
	if (map[tx][ty] == -1) {
		length++;
		map[tx][ty] = length;
		do {
			fx = rand() & 0x07;
			fy = rand() & 0x07;
		} while (i++ < 20 && map[fx][fy]);
		if (map[fx][fy]) {
			for (i = 0; i < 8; i++) {
				for (j = 0; j < 8; j++) {
					if (!map[i][j]) {
						fx = i;
						fy = j;
						goto out;
					}
				}
			}
		}
	out:
		if (map[fx][fy]) {
			return length - dlength;
		}
		map[fx][fy] = -1;
	}
	else if (map[tx][ty]) {
		return length - dlength;
	}
	else {
		for (i = 0; i < 8; i++) {
			for (j = 0; j < 8; j++) {
				if (map[i][j] > 0) {
					map[i][j]--;
				}
			}
		}
		map[tx][ty] = length;
	}
	return 0;
}