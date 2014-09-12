/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#include "ocher/ux/fb/ClockIcon.h"
#include "ocher/ux/fb/Widgets.h"

ClockIcon::ClockIcon(int x, int y) :
	Widget(x, y, 30, 30),
	m_active(false)
{
}

void ClockIcon::draw()
{
	// TODO
}

void ClockIcon::onActivate(bool active)
{
	m_active = active;
}

