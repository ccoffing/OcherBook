/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#include "clc/storage/Path.h"
#include "ocher/fmt/Format.h"

SUITE(Epub)
{
	class EpubBuilder : public FileCache
	{
	public:
		EpubBuilder() : m_root(new TreeDirectory("."))
		{
			m_root->createFilepp("mimetype", "application/epub+zip");
		}

		~EpubBuilder()
		{
			delete m_root;
		}

		TreeFile* getFile(const char* filename, const char* relative=0)
		{
			clc::Buffer fullPath;
			if (relative) {
				fullPath = clc::Path::join(relative, filename);
				filename = fullPath.c_str();
			}
			return m_root->findFile(filename);
		}

		TreeDirectory* getRoot()
		{
			return m_root;
		}

		void createContainerXml(const char* contentOpfPath)
		{
			TreeDirectory* metaInfDir = m_root->createDirectory("META-INF");
			clc::Buffer containerXml(1,
				"<?xml version=\"1.0\"?>"
				"<container version=\"1.0\" xmlns=\"urn:oasis:names:tc:opendocument:xmlns:container\">"
				"<rootfiles>"
				"<rootfile full-path=\"%s\" media-type=\"application/oebps-package+xml\"/>"
				"</rootfiles>"
				"</container>", contentOpfPath);
			metaInfDir->createFilep("container.xml", containerXml);
		}

		void createContentOpf(TreeDirectory* containingDir, const char* title)
		{
			clc::Buffer contentOpf(1,
				"<?xml version=\"1.0\"?>"
				"<package xmlns=\"http://www.idpf.org/2007/opf\" unique-identifier=\"BookID\" version=\"2.0\">"
				"    <metadata xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:opf=\"http://www.idpf.org/2007/opf\">"
				"        <dc:title>%s</dc:title>"
				"    </metadata>"
				"    <manifest>"
				"        <item id=\"ncx\" href=\"toc.ncx\" media-type=\"application/x-dtbncx+xml\"/>"
				"    </manifest>"
				"    <spine toc=\"ncx\">"
				"    </spine>"
				"</package>", title);
			containingDir->createFilep("content.opf", contentOpf);
		}

		TreeDirectory* m_root;
	};

	TEST(Detect)
	{
		PRINT_PROGRESS;

		EpubBuilder* c = new EpubBuilder;
		c->createContainerXml("content.opf");
		TreeDirectory* root = c->m_root;
		TreeDirectory* textDir = root->createDirectory("text");
		c->createContentOpf(root, "Testing Book");

		Epub epub(c);
	}
}
