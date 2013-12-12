/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#ifndef OCHER_BROWSE_CURSES_H
#define OCHER_BROWSE_CURSES_H

#include <cdk.h>

#include "clc/data/Buffer.h"

#include "ocher/ux/Browse.h"


class BrowseCurses : public Browse
{
public:
	BrowseCurses();
	~BrowseCurses() {}

	bool init(CDKSCREEN* screen);
	Meta* browse(clc::List* meta);
	void read(Meta* meta);

protected:
	CDKSCREEN* m_screen;
};


#endif


