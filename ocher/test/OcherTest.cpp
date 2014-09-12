/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#undef CLC_LOG_LEVEL
#define CLC_LOG_LEVEL 5

#include "ocher/device/Battery.h"
#include "ocher/device/Device.h"
#include "ocher/fmt/Format.h"
#include "ocher/fmt/epub/TreeMem.h"
#include "ocher/settings/Options.h"
#include "ocher/ux/PowerSaver.h"
#include "ocher/Container.h"

#include "clc/support/LogAppenders.h"
#include "clc/storage/Path.h"

#define CATCH_CONFIG_MAIN
#include "clc/test/catch.hpp"

#define REQUIRE_RESOURCES_DIR \
	if (! resourcesDir) { \
		printf("\tSpecify resources directory on command line; skipping test...\n"); \
		return; \
	}

char* resourcesDir;
Options opt;

TEST_CASE("PowerSaver")
{
}

TEST_CASE("Container construction", "[Container]")
{
	Container c;

	SECTION("nothing")
	{
	}

	SECTION("default init")
	{
		// TODO
	}

	SECTION("default init and inject")
	{
		// TODO
		//c.inject();
	}
}

TEST_CASE("Battery Capacity", "[Battery]")
{
	Battery b;
	int r;

	r = b.readCapacity();
	if (r == -1) {
		CHECK(b.m_percent == -1);
	} else {
		CHECK(b.m_percent >= 0) ;
		CHECK(b.m_percent <= 100);
	}
}

TEST_CASE("Battery Status", "[Battery]")
{
	Battery b;
	int r;

	r = b.readStatus();
	if (r == -1) {
		CHECK(b.m_status == Battery::Unknown);
	} else {
		if (b.m_status != Battery::Charging && b.m_status != Battery::Discharging)
			CHECK(b.m_status == Battery::Charging);  // catch only does binary comparisons
	}
}

TEST_CASE("Detect format")
{
	REQUIRE_RESOURCES_DIR;

	clc::Buffer fn;
	Encoding enc;
	Fmt fmt;

	fn = resourcesDir;
	fn += "/";
	fn += "simple.txt";
	fmt = detectFormat(fn.c_str(), &enc);
	CHECK(enc == OCHER_ENC_UTF8);
	CHECK(fmt == OCHER_FMT_TEXT);

	fn = resourcesDir;
	fn += "/";
	fn += "simple.bin";
	fmt = detectFormat(fn.c_str(), &enc);
	CHECK(enc == OCHER_ENC_UNKNOWN);
	CHECK(fmt == OCHER_FMT_UNKNOWN);
	fn = resourcesDir;

	fn += "/";
	fn += "simple.html";
	fmt = detectFormat(fn.c_str(), &enc);
	CHECK(enc == OCHER_ENC_UTF8);
	CHECK(fmt == OCHER_FMT_HTML);

	// epub
}

TEST_CASE("Epub")
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

	SECTION("Detect")
	{
		EpubBuilder* c = new EpubBuilder;
		c->createContainerXml("content.opf");
		TreeDirectory* root = c->m_root;
		TreeDirectory* textDir = root->createDirectory("text");
		(void)textDir; // TODO
		c->createContentOpf(root, "Testing Book");

		Epub epub(c);
	}
}
