/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#ifndef OCHER_UX_FB_TYPES_H
#define OCHER_UX_FB_TYPES_H

#include <stdint.h>


struct Pos {
	int16_t x, y;

	Pos() {}
	Pos(int16_t _x, int16_t _y) : x(_x), y(_y) {}
};

struct Rect {
	int16_t x, y;
	uint16_t w, h;

	Rect() { setInvalid(); }
	Rect(int16_t _x, int16_t _y, uint16_t _w, uint16_t _h) : x(_x), y(_y), w(_w), h(_h) {}
	Pos* pos() { return (Pos*)this; }
	void offsetBy(Pos* p) { x += p->x; y += p->y; }
	void unionRect(Rect* r);
	void unionRects(Rect* r1, Rect* r2);
	bool contains(Pos* p) { return p->x >= x && p->y >= y && p->x < x+w && p->y < y+h; }
	bool valid() { return x+w >= 0 && y+h >= 0; }
	void setInvalid() { x = y = -1; w = h = 0; }
	void inset(int i) { x+=i; y+=i; w-=(i<<1); h-=(i<<1); }
};

class Glyph
{
public:
	Glyph() : bitmap(0) {}
	~Glyph() { delete[] bitmap; }
	uint8_t* bitmap;
	uint8_t w;
	uint8_t h;
	int8_t offsetX;
	int8_t offsetY;
	int8_t advanceX;
	int8_t advanceY;
	uint8_t height;
} __attribute__((packed));

struct GlyphDescr
{
	uint32_t c;
	// TODO: bitfield
	uint8_t faceId;
	uint8_t points;
	uint8_t underline;
	uint8_t bold;
	uint8_t italic;
} __attribute__((packed));

#endif
