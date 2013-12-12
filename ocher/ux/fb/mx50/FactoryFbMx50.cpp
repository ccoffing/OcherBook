/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#include "ocher/device/kobo/KoboEvents.h"
#include "ocher/ocher.h"
#include "ocher/ux/fb/mx50/FactoryFbMx50.h"

UX_DRIVER_REGISTER(FbMx50);


UiFactoryFbMx50::UiFactoryFbMx50() :
	UiFactoryFb()
{
}

UiFactoryFbMx50::~UiFactoryFbMx50()
{
}

bool UiFactoryFbMx50::init()
{
	Mx50Fb* fb = new Mx50Fb;
	if (fb->init()) {
		m_fb = fb;
		if (UiFactoryFb::init()) {
			return true;
		}
	}
	delete fb;
	return false;
}

const char* UiFactoryFbMx50::getName()
{
	return "kobo-mx50";
}

