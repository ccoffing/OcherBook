/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#ifndef OCHER_UX_FB_NAVBAR_H
#define OCHER_UX_FB_NAVBAR_H

#include "ocher/ux/fb/Widgets.h"


class NavBar : public Window
{
public:
	NavBar(FrameBuffer* fb);

protected:
	FrameBuffer* m_fb;
	void drawContent(Rect*);
};

#endif
