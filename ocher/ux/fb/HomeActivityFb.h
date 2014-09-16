/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#ifndef OCHER_UX_FB_HOMEACTIVITY_H
#define OCHER_UX_FB_HOMEACTIVITY_H

#include "ocher/ux/fb/ActivityFb.h"


class HomeActivityFb : public ActivityFb
{
public:
	HomeActivityFb(UxControllerFb* c);
	~HomeActivityFb();

	void draw();

	int evtKey(struct OcherKeyEvent*);
	int evtMouse(struct OcherMouseEvent*);

protected:
	void onAttached();
	void onDetached();

	float coverRatio;
#define NUM_CLUSTER_BOOKS 5
	Rect books[NUM_CLUSTER_BOOKS];
	Rect shortlist[5];
	Rect m_browseLabel;
};

#endif
