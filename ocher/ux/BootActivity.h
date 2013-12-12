/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#ifndef OCHER_UX_BOOTACTIVITY_H
#define OCHER_UX_BOOTACTIVITY_H

#include "ocher/ux/Activity.h"
#include "ocher/ux/fb/Widgets.h"


class BootActivity : public Window
{
public:
	BootActivity(Controller* c);
	~BootActivity();

	void draw();

	int evtMouse(struct OcherMouseEvent*);

protected:
	void onAttached();
	void onDetached();

	Controller* m_controller;

	void highlight(int i);
	Rect apps[2];
};

#endif


