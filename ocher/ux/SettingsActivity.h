/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#ifndef OCHER_UX_SETTINGSACTIVITY_H
#define OCHER_UX_SETTINGSACTIVITY_H

#include "ocher/ux/Activity.h"


class SettingsActivity : public Window
{
public:
	SettingsActivity(Controller* c);

protected:
	void onAttached();
	void onDetached();

	Controller* m_controller;
};

#endif

