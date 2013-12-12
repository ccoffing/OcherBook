/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#ifndef OCHER_FMT_H
#define OCHER_FMT_H

#include "clc/data/Buffer.h"
#include "ocher/shelf/Meta.h"


/**
 *  Base class for file-format readers.
 */
class Format
{
public:
	virtual ~Format() {}
	virtual void loadMeta(Meta*) {}

	clc::Buffer m_title;
	clc::Buffer m_author;
};

Fmt detectFormat(const char* file, Encoding* encoding);

#include "ocher/fmt/text/Text.h"
#include "ocher/fmt/epub/Epub.h"

#endif
