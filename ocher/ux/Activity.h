/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#ifndef OCHER_UX_ACTIVITY_H
#define OCHER_UX_ACTIVITY_H


enum ActivityType {
	ACTIVITY_BOOT,
	ACTIVITY_SLEEP,
	ACTIVITY_SYNC,
	ACTIVITY_HOME,
	ACTIVITY_READ,
	ACTIVITY_LIBRARY,
	ACTIVITY_SETTINGS,

	ACTIVITY_PREVIOUS,
	ACTIVITY_QUIT,
};


/**
 * A logical portion of user interaction.
 */
class Activity
{
public:
	virtual void onAttached() = 0;
	virtual void onDetached() = 0;
};

#endif
