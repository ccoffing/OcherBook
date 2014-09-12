/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#ifndef OCHER_UX_FB_BOOTACTIVITY_H
#define OCHER_UX_FB_BOOTACTIVITY_H

#include "ocher/ux/fb/ActivityFb.h"


class BootActivityFb : public ActivityFb
{
public:
	BootActivityFb(UxControllerFb* uxController);
	~BootActivityFb();

	void draw();

	int evtMouse(struct OcherMouseEvent*);

	void onAttached();
	void onDetached();

protected:
	FrameBuffer* m_fb;

	void highlight(int i);
	Rect apps[2];
};

#endif
