/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#ifndef MX50_FB_H
#define MX50_FB_H

#include "ocher/device/kobo/KoboEvents.h"
#include "ocher/ux/fb/FrameBuffer.h"

#include <linux/mxcfb.h>


class FrameBufferMx50 : public FrameBuffer
{
public:
	FrameBufferMx50();
	~FrameBufferMx50();

	bool init();

	unsigned int height();
	unsigned int width();
	unsigned int dpi();

	void inject(EventLoop* loop) { m_events.setEventLoop(loop); }

	void setFg(uint8_t r, uint8_t b, uint8_t g);
	void setBg(uint8_t r, uint8_t b, uint8_t g);
	void clear();
	void pset(int x, int y);
	void hline(int x1, int y, int x2);
	inline void vline(int x, int y1, int y2) { line(x, y1, x, y2); }
	void blit(const unsigned char* p, int x, int y, int w, int h, const Rect* clip);
	void fillRect(Rect* r);
	void byLine(Rect* r, void(*fn)(void* p, size_t n));
	int update(Rect* r, bool full=false);
	void sync();
	void needFull() { m_needFull = true; }

	/**
	 * @param marker  Waits on the specified update, or -1 for all
	 */
	void waitUpdate(int marker = -1);

	void setPixelFormat();
	void setUpdateScheme();
	void setAutoUpdateMode(bool autoUpdate);

protected:
	KoboEvents m_events;  // TODO abstract

	int m_fd;
	char* m_fb;
	size_t m_fbSize;
	unsigned int m_marker;
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;
	uint8_t m_fgColor;
	uint8_t m_bgColor;
	bool m_needFull;
};

#endif
