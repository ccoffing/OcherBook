/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#include "ocher/Container.h"
#include "ocher/ux/fb/NavBar.h"


NavBar::NavBar(FrameBuffer* fb) :
	Window(0, fb->height()-50, fb->width(), 50),
	m_fb(fb)
{
}

void NavBar::drawContent(Rect* pos)
{
	m_rect.y++;
	m_rect.h--;
	m_fb->setFg(0xff, 0xff, 0xff);
	m_fb->fillRect(&m_rect);
	m_rect.y--;
	m_rect.h++;
	m_fb->setFg(0, 0, 0);
	m_fb->hline(m_rect.x, m_rect.y, m_rect.x+m_rect.w-1);
}
