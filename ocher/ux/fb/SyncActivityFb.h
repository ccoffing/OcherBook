/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#ifndef OCHER_UX_SYNCACTIVITY_H
#define OCHER_UX_SYNCACTIVITY_H

#include "ocher/ux/fb/ActivityFb.h"


class SyncActivityWork;

class SyncActivityFb : public ActivityFb
{
public:
	SyncActivityFb(UxControllerFb* c);

	void draw();

protected:
	void onAttached();
	void onDetached();

	FrameBuffer* m_fb;
	SyncActivityWork* m_work;
	Spinner m_spinner;
};

#endif

