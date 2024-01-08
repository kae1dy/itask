/*
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "quakegeneric.h"

uint32_t *rgbpixels;
unsigned char pal[768];

#define ARGB(r, g, b, a) (((a) << 24) | ((r) << 16) | ((g) << 8) | (b))

#define KEYBUFFERSIZE	32
static int keybuffer[KEYBUFFERSIZE];  // circular key buffer
static int keybuffer_len;  // number of keys in the buffer
static int keybuffer_start;  // index of next item to be read

void QG_Init(void)
{
	// TODO: like SDL impl
	rgbpixels = malloc(QUAKEGENERIC_RES_X * QUAKEGENERIC_RES_Y * sizeof(uint32_t));

	keybuffer_len = 0;
	keybuffer_start = 0;
}


static int ConvertToQuakeKey(unsigned int key)
{
	int qkey;
	// TODO: like SDL impl
}

static int KeyPop(int *down, int *key)
{
	if (keybuffer_len == 0)
		return -1; // underflow EXIT_FAILURE

	*key = keybuffer[keybuffer_start];
	*down = *key < 0;
	if (*key < 0)
		*key = -*key;
	keybuffer_start = (keybuffer_start + 1) % KEYBUFFERSIZE;
	keybuffer_len--;
	return 0;
}

static int KeyPush(int down, int key)
{
	if (keybuffer_len == KEYBUFFERSIZE)
		return -1; // overflow EXIT_FAILURE
	if (down) {
		key = -key;
	}
	keybuffer[(keybuffer_start + keybuffer_len) % KEYBUFFERSIZE] = key;
	keybuffer_len++;
	return 0;
}

int QG_GetKey(int *down, int *key)
{
	return KeyPop(down, key);
}

void QG_Quit(void)
{
	// TODO: like SDL impl
}

#define ASPECT_WH ((float)(QUAKEGENERIC_RES_X) / (float)(QUAKEGENERIC_RES_Y))
#define ASPECT_HW ((float)(QUAKEGENERIC_RES_Y) / (float)(QUAKEGENERIC_RES_X))


void QG_DrawFrame(void *pixels)
{
	// TODO: like SDL impl
}

void QG_SetPalette(unsigned char palette[768])
{
	memcpy(pal, palette, 768);
}

void umain(int argc, char **argv)
{
	double oldtime, newtime;
	int running = 1;
	int button;

	QG_Create(argc, argv);

	// TODO: like SDL impl
}
