/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#ifndef OCHER_CSS_H
#define OCHER_CSS_H

#include <libcss/libcss.h>
#include "clc/data/Buffer.h"

class CssParser
{
public:
	CssParser();
	~CssParser();

	void parseString(clc::Buffer &b);

	css_stylesheet *sheet;
};

#endif
