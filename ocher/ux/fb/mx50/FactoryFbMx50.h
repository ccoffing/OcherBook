/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#ifndef OCHER_UX_FACTORY_FB_MX50_H
#define OCHER_UX_FACTORY_FB_MX50_H

#include "ocher/ux/fb/FactoryFb.h"
#include "ocher/ux/fb/mx50/fb.h"


class UiFactoryFbMx50 : public UiFactoryFb
{
public:
	UiFactoryFbMx50();
	~UiFactoryFbMx50();

	bool init();
	const char* getName();
};

#endif

