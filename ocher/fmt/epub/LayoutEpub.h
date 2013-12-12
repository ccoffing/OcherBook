/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#ifndef OCHER_FMT_EPUB_LAYOUT_H
#define OCHER_FMT_EPUB_LAYOUT_H

#include "ocher/fmt/Layout.h"


class Epub;

class LayoutEpub : public Layout
{
public:
	LayoutEpub(Epub* epub) : m_epub(epub) {}

	void append(mxml_node_t* tree);

protected:
	void processNode(mxml_node_t* node);
	void processSiblings(mxml_node_t* node);

	Epub* m_epub;
};

#endif
