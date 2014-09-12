/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#ifndef OCHER_UX_CDK_BROWSEACTIVITY_H
#define OCHER_UX_CDK_BROWSEACTIVITY_H

#include "ocher/ux/cdk/ActivityCdk.h"

#include "clc/data/Buffer.h"


class BrowseActivityCdk : public ActivityCdk
{
public:
	BrowseActivityCdk(UxControllerCdk* uxController);
	~BrowseActivityCdk() {}

	void onAttached();
	void onDetached();
};


#endif


