/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#include <stdio.h>

#include "mxml.h"

#include "clc/storage/Path.h"
#include "clc/support/Logger.h"
#include "ocher/fmt/epub/Epub.h"

#define LOG_NAME "ocher.epub"


static bool stripUtf8Bom(clc::Buffer &data)
{
	// Unicode standard does not recommend BOM for UTF8.
	// UTF8 is assumed anyway.
	if (data.length() >= 3 &&
			((unsigned char*)data.c_str())[0] == 0xef &&
			((unsigned char*)data.c_str())[1] == 0xbb &&
			((unsigned char*)data.c_str())[2] == 0xbf) {
		data.remove(0, 3);
		return true;
	}
	return false;
}

TreeFile* Epub::findSpine()
{
	TreeFile* mimetype = m_zip->getFile("mimetype");
	if (!mimetype) {
		clc::Log::warn(LOG_NAME, "Missing '/mimetype'");
	} else {
		stripUtf8Bom(mimetype->data);
		if (mimetype->data.length() < 20 ||
				strncmp(mimetype->data.c_str(), "application/epub+zip", 20)) {
			clc::Log::warn(LOG_NAME, "'/mimetype' has incorrect value: '%s' (%u)",
					mimetype->data.c_str(), (unsigned int)mimetype->data.length());
		}
	}

	mxml_node_t* tree = 0;
	const char* fullPath = 0;
	TreeFile* container = m_zip->getFile("META-INF/container.xml");
	if (! container) {
		clc::Log::error(LOG_NAME, "Missing 'META-INF/container.xml'");
	} else {
		stripUtf8Bom(container->data);
		tree = mxmlLoadString(NULL, container->data.c_str(), MXML_IGNORE_CALLBACK);
		// Must be a "rootfiles" element, with one or more "rootfile" children.
		// First "rootfile" is the default. [OCF 3.0 2.5.1]
		mxml_node_t* rootfile = mxmlFindPath(tree, "container/rootfiles/rootfile");
		if (! rootfile) {
			clc::Log::error(LOG_NAME, "Missing rootfile node");
		} else {
			clc::Log::trace(LOG_NAME, "found default rootfile");
			fullPath = mxmlElementGetAttr(rootfile, "full-path");
			if (!fullPath)
				clc::Log::error(LOG_NAME, "Missing 'full-path' attr");
			else {
				clc::Log::trace(LOG_NAME, "Found 'full-path' attr: '%s'", fullPath);
				clc::Buffer textPath(fullPath);
				// TODO:  path handling is weak: canonicalization, ...
				m_contentPath = clc::Path::getDirectory(textPath);
				if (m_contentPath == textPath) {
					m_contentPath = "";
				}
			}
		}
	}

	TreeFile* spine = 0;
	if (fullPath) {
		spine = m_zip->getFile(fullPath);
		if (! spine)
			clc::Log::error(LOG_NAME, "Missing spine '%s'", fullPath);
		else
			clc::Log::trace(LOG_NAME, "Found spine '%s'", fullPath);
	}
	if (tree)
		mxmlDelete(tree);
	return spine;
}

const char* _mxmlElementGetAttr(mxml_node_t* n, const char* name)
{
	clc::Log::trace(LOG_NAME, "attrs %d", n->value.element.num_attrs);
	for (int i = 0; i < n->value.element.num_attrs; ++i) {
		if (strcmp(n->value.element.attrs[i].name, name) == 0) {
			clc::Log::trace(LOG_NAME, "match %s", n->value.element.attrs[i].name);
			return n->value.element.attrs[i].value;
		}
		clc::Log::trace(LOG_NAME, "mismatch %s", n->value.element.attrs[i].name);
	}
	return NULL;
}

void Epub::parseSpine(TreeFile* spineFile)
{
	stripUtf8Bom(spineFile->data);

	mxml_node_t* tree = mxmlLoadString(NULL, spineFile->data.c_str(), MXML_IGNORE_CALLBACK);

	mxml_node_t* package = mxmlFindPath(tree, "package");
	if (!package) {
		clc::Log::warn(LOG_NAME, "Missing 'package' element");
	} else {
		clc::Log::trace(LOG_NAME, "Found 'package' type %d", package->type);
		m_uid = _mxmlElementGetAttr(package, "unique-identifier");
		m_epubVersion = _mxmlElementGetAttr(package, "version");
	}

	mxml_node_t* metadata = mxmlFindPath(tree, "package/metadata");
	if (! metadata) {
		clc::Log::warn(LOG_NAME, "Missing 'metadata' element");
	} else {
		clc::Log::debug(LOG_NAME, "Found 'metadata'");
		for (mxml_node_t* node = mxmlGetFirstChild(metadata); node; node = mxmlGetNextSibling(node)) {
			const char* name = node->value.element.name;
			if (node->type == MXML_ELEMENT) {
				if (strcasecmp(name, "dc:title") == 0) {
					clc::Log::debug(LOG_NAME, "Found dc:title");
					//m_title = mxmlGetNextSibling(node)->value.opaque;
				} else if (strcasecmp(name, "dc:creator") == 0) {
					// dc:creator opf:file-as="" opf:role="aut"></dc:creator>
					//m_author = mxmlGetNextSibling(node)->value.opaque;
				} else if (strcasecmp(name, "dc:language") == 0) {
					// <dc:language>en-US</dc:language>
				}
			}
		}
		m_title = metadata->value.opaque;
	}

	mxml_node_t* manifest = mxmlFindPath(tree, "package/manifest");
	if (! manifest) {
		clc::Log::warn(LOG_NAME, "Missing 'manifest' element");
	} else {
		clc::Log::trace(LOG_NAME, "Found 'manifest' type %d", manifest->type);
		for (mxml_node_t* i = manifest->child; i; i = i->next) {
			if (i->type != MXML_ELEMENT || strcmp(i->value.element.name, "item"))
				continue;
			clc::Log::trace(LOG_NAME, "Found 'manifest' item");
			const char* id = _mxmlElementGetAttr(i, "id");
			const char* href = _mxmlElementGetAttr(i, "href");
			const char* mediaType = _mxmlElementGetAttr(i, "media-type");
			if (id && href) {
				EpubItem item;
				item.href = href;
				item.mediaType = mediaType;
				clc::Log::trace(LOG_NAME, "%s -> %s", id, href);
				clc::Buffer _id(id);
				m_items.insert(std::pair<clc::Buffer, EpubItem>(_id, item));
			}
		}
	}

	mxml_node_t* spine = mxmlFindPath(tree, "package/spine");
	if (! spine) {
		clc::Log::warn(LOG_NAME, "Missing 'spine' element");
	} else {
		clc::Log::trace(LOG_NAME, "Found 'spine'");
		// TODO: toc
		for (mxml_node_t* i = spine->child; i; i = i->next) {
			if (i->type != MXML_ELEMENT || strcmp(i->value.element.name, "itemref"))
				continue;
			const char* idref = _mxmlElementGetAttr(i, "idref");
			if (idref) {
				clc::Buffer _idref(idref);
				m_spine.push_back(_idref);
			}
		}
	}
	if (tree)
		mxmlDelete(tree);
}

int Epub::getSpineItemByIndex(unsigned int i, clc::Buffer &item)
{
	if (i < m_spine.size()) {
		clc::Buffer &idref = m_spine[i];
		std::map<clc::Buffer,EpubItem>::iterator it = m_items.find(idref);
		if (it != m_items.end()) {
			TreeFile* f = m_zip->getFile((*it).second.href.c_str(), m_contentPath.c_str());
			if (f) {
				item = f->data;
				return 0;
			}
		}
	}
	clc::Log::warn(LOG_NAME, "Missing spine item #%d", i);
	return -1;
}

int Epub::getManifestItemById(unsigned int i, clc::Buffer &item)
{
	// TODO
	(void)i;
	(void)item;
	return -1;
}

int Epub::getContentByHref(const char* href, clc::Buffer &item)
{
	// TODO
	(void)href;
	(void)item;
	return -1;
}

Epub::Epub(FileCache* fileCache) :
	m_zip(fileCache)
{
	TreeFile* spine = findSpine();
	if (spine) {
		parseSpine(spine);
	}
}

Epub::Epub(const char* filename, const char* password) :
	m_zip(new UnzipCache(filename, password))
{
	TreeFile* spine = findSpine();
	if (spine) {
		parseSpine(spine);
	}
}

Epub::~Epub()
{
	delete m_zip;
}

mxml_node_t* Epub::parseXml(clc::Buffer &xml)
{
	mxml_node_t* tree = mxmlLoadString(NULL, xml.c_str(), MXML_OPAQUE_CALLBACK);
	return tree;
}
