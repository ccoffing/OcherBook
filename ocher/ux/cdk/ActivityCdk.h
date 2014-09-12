/*
 * Copyright (c) 2014, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#ifndef OCHER_UX_CDK_ACTIVITY_H
#define OCHER_UX_CDK_ACTIVITY_H

#include "ocher/ux/Activity.h"

class UxControllerCdk;

/**
 */
class ActivityCdk : public Activity
{
public:
	ActivityCdk(UxControllerCdk* uxController) : m_uxController(uxController) {}
	virtual ~ActivityCdk() {}

	virtual void onAttached() = 0;
	virtual void onDetached() = 0;

	UxControllerCdk* m_uxController;
};

#endif
