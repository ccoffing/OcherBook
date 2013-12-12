/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#ifndef OCHER_EPUB_PARSER_H
#define OCHER_EPUB_PARSER_H

#include <map>
#include <vector>

#include "mxml.h"

#include "clc/data/Buffer.h"
#include "ocher/fmt/Format.h"
#include "ocher/fmt/epub/UnzipCache.h"


struct EpubItem
{
	clc::Buffer href;
	clc::Buffer mediaType;
};

class Epub : public Format
{
public:
	/**
	 */
	Epub(FileCache* fileCache);
	/**
	 */
	Epub(const char* epubFilename, const char* password=0);
	~Epub();

	clc::Buffer m_epubVersion;
	clc::Buffer m_uid;

	clc::Buffer getFile(const char* filename) {
		TreeFile* f = m_zip->getFile(filename, m_contentPath.c_str());
		clc::Buffer b;
		if (f) {
			b = f->data;
		}
		return b;
	}

	int getSpineItemByIndex(unsigned int i, clc::Buffer &item);
	int getManifestItemById(unsigned int i, clc::Buffer &item);
	int getContentByHref(const char* href, clc::Buffer &item);

	/**
	 * Parses XML. Caller must call mxml_delete.
	 */
	mxml_node_t* parseXml(clc::Buffer &xml);

protected:
	TreeFile* findSpine();
	void parseSpine(TreeFile* spine);

	FileCache* m_zip;
	std::map<clc::Buffer, EpubItem> m_items;
	std::vector<clc::Buffer> m_spine;
	clc::Buffer m_contentPath;  ///< directory of full-path attr
};


#endif
