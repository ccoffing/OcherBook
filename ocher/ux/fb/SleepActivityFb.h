/*
 * Copyright (c) 2014, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#ifndef OCHER_UX_FB_SLEEPACTIVITY_H
#define OCHER_UX_FB_SLEEPACTIVITY_H

#include "ocher/ux/fb/ActivityFb.h"


class SleepActivityFb : public ActivityFb
{
public:
	SleepActivityFb(UxControllerFb* c);

	void onAttached();
	void onDetached();

protected:
	UxControllerFb* m_uxController;
	FrameBuffer* m_fb;
	Device* m_device;
	EventLoop* m_loop;
};

#endif
