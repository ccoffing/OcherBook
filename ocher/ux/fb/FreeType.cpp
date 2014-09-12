/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#include "ocher/Container.h"
#include "ocher/settings/Settings.h"
#include "ocher/ux/fb/FreeType.h"

#include "clc/support/Logger.h"

#include <ctype.h>

#define LOG_NAME "ocher.freetype"



static const char* ttfFiles[] = {
	// TODO
#ifdef OCHER_TARGET_KOBO
	"Delima.ttf",
	"Delima-Italic.ttf",
	"Delima-Bold.ttf",
	"Delima-BoldItalic.ttf",
#else
#if 0
	"liberation/LiberationSerif-Regular.ttf",
	"liberation/LiberationSerif-Italic.ttf",
	"liberation/LiberationSerif-Bold.ttf",
	"liberation/LiberationSerif-BoldItalic.ttf",
#else
	"dejavu/DejaVuSerif.ttf",
	"dejavu/DejaVuSerif-Italic.ttf",
	"dejavu/DejaVuSerif-Bold.ttf",
	"dejavu/DejaVuSerif-BoldItalic.ttf",
#endif
#endif
};

FreeType::FreeType(unsigned int dpi) :
	m_lib(0),
	m_dpi(dpi)
{
}

FreeType::~FreeType()
{
	FT_Done_FreeType(m_lib);
}

bool FreeType::init()
{
	int r;
	r = FT_Init_FreeType(&m_lib);
	if (r) {
		clc::Log::error(LOG_NAME, "FT_Init_FreeType failed: %d", r);
		return false;
	}
	return setFace(0, 0);
}

bool FreeType::setFace(int i, int b)
{
	clc::Buffer file = g_container.settings->fontRoot;
	i = i ? 1 : 0;
	b = b ? 1 : 0;
	file += "/";
	file += ttfFiles[i+b*2];

	int r = FT_New_Face(m_lib, file.c_str(), 0, &m_face);
	if (r || !m_face) {
		clc::Log::error(LOG_NAME, "FT_New_Face(\"%s\") failed: %d", file.c_str(), r);
		return false;
	}
	return true;
}

void FreeType::setSize(unsigned int points)
{
	FT_Set_Char_Size(m_face, 0, points*64, m_dpi, m_dpi);
}

int FreeType::plotGlyph(GlyphDescr* f, Glyph* g)
{
	unsigned int glyphIndex = FT_Get_Char_Index(m_face, f->c);
	int r = FT_Load_Glyph(m_face, glyphIndex, FT_LOAD_DEFAULT);
	if (r) {
		clc::Log::error(LOG_NAME, "FT_Load_Glyph failed: %d", r);
		return -1;
	}
	if (m_face->glyph->format != FT_GLYPH_FORMAT_BITMAP) {
		r = FT_Render_Glyph(m_face->glyph, FT_RENDER_MODE_NORMAL);
		if (r) {
			clc::Log::error(LOG_NAME, "FT_Render_Glyph failed: %d", r);
			return -1;
		}
	}

	FT_GlyphSlot slot = m_face->glyph;
	g->w = slot->bitmap.width;
	g->h = slot->bitmap.rows;
	g->bitmap = new uint8_t[g->w * g->h];
	memcpy(g->bitmap, slot->bitmap.buffer, g->w * g->h);
	g->offsetX = slot->bitmap_left;
	g->offsetY = slot->bitmap_top;
	g->advanceX = slot->advance.x >> 6;
	g->advanceY = slot->advance.y >> 6;
	g->height = m_face->size->metrics.height >> 6;
	return 0;
}
