/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#ifndef OCHER_UX_FB_READACTIVITY_H
#define OCHER_UX_FB_READACTIVITY_H

#include "ocher/ux/fb/ActivityFb.h"


class Renderer;
class Layout;

class ReadActivityFb : public ActivityFb
{
public:
	ReadActivityFb(UxControllerFb* c);

protected:
	int evtKey(struct OcherKeyEvent*);
	int evtMouse(struct OcherMouseEvent*);

	void onAttached();
	void onDetached();

	void draw();

	FrameBuffer* m_fb;
	Settings* m_settings;

	Layout* m_layout;
	Renderer* m_renderer;
	Meta* meta;
	unsigned int m_pageNum;
	int atEnd;
	unsigned int m_pagesSinceRefresh;
};

#endif
