/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#ifndef OCHER_UX_FB_SYSTEMBAR_H
#define OCHER_UX_FB_SYSTEMBAR_H

#include "ocher/ux/fb/Widgets.h"
#include "ocher/ux/fb/BatteryIcon.h"
#include "ocher/ux/fb/ClockIcon.h"

#include "clc/data/Buffer.h"


class SystemBar : public Window
{
public:
	SystemBar(FrameBuffer* fb, Battery* battery);

	bool m_sep;
	clc::Buffer m_title;

protected:
	void drawContent(Rect*);

	FrameBuffer* m_fb;

	BatteryIcon m_batteryIcon;
	ClockIcon m_clockIcon;
};

#endif
