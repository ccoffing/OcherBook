/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#ifndef OCHER_UX_FB_CLOCKICON_H
#define OCHER_UX_FB_CLOCKICON_H

#include "ocher/ux/fb/Widgets.h"


class ClockIcon : public Widget
{
public:
	ClockIcon(int x, int y);

	void draw();

	void onActivate(bool active);

protected:
	bool m_active;
};

#endif
