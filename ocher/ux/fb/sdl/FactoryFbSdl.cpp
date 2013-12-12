/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#include "ocher/ocher.h"
#include "ocher/ux/fb/sdl/FactoryFbSdl.h"
#include "ocher/ux/fb/sdl/SdlLoop.h"


UX_DRIVER_REGISTER(FbSdl);


UiFactoryFbSdl::UiFactoryFbSdl() :
	UiFactoryFb()
{
}

UiFactoryFbSdl::~UiFactoryFbSdl()
{
}

bool UiFactoryFbSdl::init()
{
	FbSdl* fbSdl = new FbSdl();
	if (fbSdl->init()) {
		m_fb = fbSdl;
		if (UiFactoryFb::init()) {
			return true;
		}
	}
	delete fbSdl;
	return false;
}

const char* UiFactoryFbSdl::getName()
{
	return "sdl";
}


