/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#ifndef OCHER_UX_FACTORY_FB_SDL_H
#define OCHER_UX_FACTORY_FB_SDL_H

#include "ocher/ux/fb/FactoryFb.h"
#include "ocher/ux/fb/sdl/FbSdl.h"


class UiFactoryFbSdl : public UiFactoryFb
{
public:
	UiFactoryFbSdl();
	~UiFactoryFbSdl();

	bool init();
	const char* getName();
};

#endif

