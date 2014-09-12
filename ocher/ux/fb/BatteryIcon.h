/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#ifndef OCHER_UX_FB_BATTERYICON_H
#define OCHER_UX_FB_BATTERYICON_H

#include "ocher/ux/fb/Widgets.h"

class Battery;

class BatteryIcon : public Widget
{
public:
	BatteryIcon(int x, int y, Battery* battery);

	void draw();

	void onUpdate();

	Battery* m_battery;
};

#endif

