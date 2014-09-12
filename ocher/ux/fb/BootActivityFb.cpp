/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#include "ocher/Container.h"
#include "ocher/ux/fb/BootActivityFb.h"
#include "ocher/ux/fb/FontEngine.h"

#include "clc/support/Logger.h"

#include <unistd.h>

#define LOG_NAME "ocher.ux.Boot"


BootActivityFb::BootActivityFb(UxControllerFb* c) :
	ActivityFb(c),
	m_fb(g_container.frameBuffer)
{
	float ratio = 1.6;
	int w = m_fb->width();
	int h = m_fb->height();

	// Calc nice percentage widths, then calc height proportionally, then center vertically.
	apps[0].x = w / 10;
	apps[0].w = w / 10 * 3;
	apps[0].h = apps[0].w * ratio;
	apps[0].y = (h - apps[0].h) / 2;
	apps[1].x = apps[0].x + w / 2;
	apps[1].w = apps[0].w;
	apps[1].h = apps[0].h;
	apps[1].y = apps[0].y;
}

BootActivityFb::~BootActivityFb()
{
}

void BootActivityFb::onAttached()
{
}

void BootActivityFb::onDetached()
{
}

void BootActivityFb::highlight(int i)
{
	clc::Log::debug(LOG_NAME, "highlight %d", i);
	Rect r = apps[i];
	for (int n = 0; n < 4; ++n) {
		r.inset(-1);
		m_fb->rect(&r);
	}
	m_fb->update(&r);
	m_fb->sync();
}

int BootActivityFb::evtMouse(struct OcherMouseEvent* evt)
{
	if (evt->subtype == OEVT_MOUSE1_UP) {
		Pos pos(evt->x, evt->y);
		if (apps[0].contains(&pos)) {
			highlight(0);
			return 0;
		} else if (apps[1].contains(&pos)) {
			highlight(1);
			exit(1);
		}
	}
	return -1;
}

void BootActivityFb::draw()
{
	clc::Log::debug(LOG_NAME, "draw");

	Rect r = m_fb->bbox;

	m_fb->setFg(0xff, 0xff, 0xff);
	m_fb->fillRect(&r);

	m_fb->setFg(0, 0, 0);
	FontEngine fe(m_fb);
	fe.setSize(16);
	fe.setItalic(1);
	fe.apply();

	Pos pos;
	for (int i = 0; i < 2; ++i) {
		r = apps[i];
		m_fb->rect(&r);
		r.inset(-1);
		m_fb->roundRect(&r, 1);
		r.inset(2);

		const char* label = i == 0 ? "OcherBook" : "Kobo" /* or "Nickel" ? */ ;
		pos.x = 0;
		pos.y = r.h/2;
		fe.renderString(label, strlen(label), &pos, &r, FE_XCENTER);
	}
}

