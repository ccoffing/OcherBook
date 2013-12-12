/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#ifndef SDL_FB_H
#define SDL_FB_H

#include "SDL/SDL.h"

#include "ocher/ux/fb/sdl/SdlLoop.h"
#include "ocher/ux/fb/FrameBuffer.h"


class FbSdl : public FrameBuffer
{
public:
	FbSdl();
	virtual ~FbSdl();

	bool init();

	unsigned int height();
	unsigned int width();
	unsigned int dpi();

	void setEventLoop(EventLoop* loop);
	void setFg(uint8_t r, uint8_t b, uint8_t g);
	void setBg(uint8_t r, uint8_t b, uint8_t g);
	void clear();
	void pset(int x, int y);
	void hline(int x1, int y, int x2);
	void vline(int x, int y1, int y2);
	void blit(const unsigned char* p, int x, int y, int w, int h, const Rect* clip);
	void fillRect(Rect* r);
	void byLine(Rect* r, void(*fn)(void* p, size_t n));
	int update(Rect* r, bool full=false);

protected:
	SdlLoop m_loop;

	int m_sdl;
	SDL_Surface* m_screen;
	bool m_mustLock;
	uint8_t m_fgColor;
	uint8_t m_bgColor;

	uint8_t getColor(uint8_t r, uint8_t b, uint8_t g);
};

#endif
