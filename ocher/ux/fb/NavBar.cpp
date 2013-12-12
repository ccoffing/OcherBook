/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#include "ocher/ux/Factory.h"
#include "ocher/ux/fb/NavBar.h"


NavBar::NavBar() :
	Window(0, g_fb->height()-50, g_fb->width(), 50)
{
}

void NavBar::drawContent(Rect* pos)
{
	m_rect.y++;
	m_rect.h--;
	g_fb->setFg(0xff, 0xff, 0xff);
	g_fb->fillRect(&m_rect);
	m_rect.y--;
	m_rect.h++;
	g_fb->setFg(0, 0, 0);
	g_fb->hline(m_rect.x, m_rect.y, m_rect.x+m_rect.w-1);
}
