/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#include "ocher/ux/Event.h"
#include "ocher/ux/Factory.h"

EventLoop* g_loop;
FrameBuffer* g_fb;
FreeType* g_ft;


clc::List& getDrivers()
{
	static clc::List drivers;
	return drivers;
}


UiFactory::UiFactory()
{
}

void UiFactory::populate()
{
	// Populate globals for ease of access
	g_fb = uiFactory->getFrameBuffer();
	g_ft = uiFactory->getFontEngine();
	g_loop = new EventLoop;
}
