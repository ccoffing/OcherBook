/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#include <stddef.h>
#include <ctype.h>

#include "clc/support/Logger.h"
#include "ocher/settings/Settings.h"
#include "ocher/ux/Factory.h"
#include "ocher/ux/fb/FontEngine.h"

#define LOG_NAME "ocher.ux.FontEngine"


GlyphCache::GlyphCache()
{
}

GlyphCache::~GlyphCache()
{
    clc::HashtableIter iter(m_cache);
    while (iter.hasNext())
        delete ((Glyph*)iter.next());
}

void GlyphCache::put(GlyphDescr* f, Glyph* g)
{
    m_cache.put(f, sizeof(*f), g);
}

Glyph* GlyphCache::get(GlyphDescr* f)
{
    return (Glyph*)m_cache.get(f, sizeof(*f));
}

FontEngine::FontEngine()
{
    m_next.faceId = 0;
    m_next.points = g_settings.fontPoints;
    m_next.underline = 0;
    m_next.bold = 0;
    m_next.italic = 0;
    memset(&m_cur, 0xff, sizeof(m_cur));
    dirty = 1;
}

FontEngine::~FontEngine()
{
}

void FontEngine::scanForFonts()
{
    const char* search;
    if (g_settings.fontRoot.length() == 0)
        search = ".";
    else
        search = g_settings.fontRoot.c_str();

    const char* p = search;
    const char* colon;
    colon = strchr(p, ':');
    if (colon) {
        // TODO
    }
}

static int utf8ToUtf32(const char* _p, uint32_t* u32)
{
    const unsigned char* p = (const unsigned char*)_p;
    int len = 1;
    uint32_t c = *p;
    if (c >= 0x7f) {
        if ((c & 0xe0) == 0xc0) {
            c = ((c & 0x1f) << 6) | (p[1] & 0x3f);
            len++;
        } else if ((c & 0xf0) == 0xe0) {
            c = ((c & 0x0f) << 12) | ((p[1] & 0x3f) << 6) | (p[2] & 0x3f);
            len += 2;
        } else if ((c & 0xf8) == 0xf0) {
            c = ((c & 0x07) << 18) | ((p[1] & 0x3f) << 12) | ((p[2] & 0x3f) << 6) | (p[3] & 0x3f);
            len += 3;
        } else if ((c & 0xfc) == 0xf8) {
            c = ((c & 0x03) << 24) | ((p[1] & 0x3f) << 18) | ((p[2] & 0x3f) << 12) | ((p[3] & 0x3f) << 6) | (p[4] & 0x3f);
            len += 4;
        } else if ((c & 0xfe) == 0xfc) {
            c = ((c & 0x01) << 30) | ((p[1] & 0x3f) << 24) | ((p[2] & 0x3f) << 18) | ((p[3] & 0x3f) << 12) | ((p[4] & 0x3f) << 6) | (p[5] & 0x3f);
            len += 5;
        } else {
            // out of sync?
            c = 0;
        }
    }
    *u32 = c;
    return len;
}

void FontEngine::setFont()
{
    // TODO
}

void FontEngine::setSize(unsigned int points)
{
    m_next.points = points;
    dirty = 1;
}

void FontEngine::setBold(int bold)
{
    m_next.bold = bold;
    dirty = 1;
}

void FontEngine::setUnderline(int underline)
{
    m_next.underline = underline;
    dirty = 1;
}

void FontEngine::setItalic(int italic)
{
    m_next.italic = italic;
    dirty = 1;
}

void FontEngine::apply()
{
    if (dirty) {
        if (m_cur.faceId != m_next.faceId) {
            /*TODO*/;
        }
        if (m_cur.italic != m_next.italic || m_cur.bold != m_next.bold) {
            g_ft->setFace(m_next.italic, m_next.bold);
            g_ft->setSize(m_next.points);
        } else if (m_cur.points != m_next.points) {
            g_ft->setSize(m_next.points);
        }
        if (m_cur.underline != m_next.underline) {
            /*TODO*/;
        }
        m_cur = m_next;
        m_cur.ascender = g_ft->getAscender();
        m_cur.descender = g_ft->getDescender();
        m_cur.bearingY = m_cur.ascender;  /* TODO */
        m_cur.lineHeight = g_ft->getLineHeight();
        m_cur.underlinePos = g_ft->getUnderlinePos();
        dirty = 0;
    }
}

void FontEngine::plotString(const char* p, unsigned int len, Glyph** glyphs, Rect* bbox)
{
    GlyphDescr d;
    d.faceId = m_cur.faceId;
    d.points = m_cur.points;
    d.underline = m_cur.underline;
    d.bold = m_cur.bold;
    d.italic = m_cur.italic;

    bbox->w = 0;
    unsigned int i = 0;
    for (const char* end = p+len; p < end; ) {
        p += utf8ToUtf32(p, &d.c);

        Glyph* g = m_cache.get(&d);
        if (!g) {
            //clc::Log::trace(LOG_NAME, "%d pt %c is not cached", d.face.points, d.c);
            g = new Glyph;
            if (g_ft->plotGlyph(&d, g) < 0) {
                clc::Log::warn(LOG_NAME, "plotGlyph failed for %x; skipping", d.c);
                delete g;
                continue;
            }

            invert(g->bitmap, g->w * g->h);
            m_cache.put(&d, g);
        }

        glyphs[i++] = g;
        bbox->w += g->advanceX;
    }
    bbox->h = m_cur.lineHeight;
    glyphs[i] = 0;
}

// TODO:  return array of arrays of glyphs, and starting pos for each array
// or callback:  str, offset, len, bbox
unsigned int FontEngine::renderString(const char* str, unsigned int len, Pos* pen, const Rect* r, unsigned int flags, Rect* bbox)
{
    const char* p = str;
    bool wordWrapped = false;
    if (bbox)
        bbox->setInvalid();

    if (! (flags & FE_YCLIP) && pen->y >= r->h - m_cur.descender) {
        return 0;
    }

    do {
        // If at start of line, eat spaces
        if (pen->x == 0) {
            while (len && isspace(*p)) {
                ++p;
                --len;
            }
        }

        if (*p != '\n') {
            // Where is the next word break?
            unsigned int w = 0;
            while (w < len && !isspace(*(p+w))) {
                ++w;
            }
            if (w < len)
                ++w;

            Glyph* glyphs[w+1];
            Rect wordBox;
            wordBox.x = pen->x;
            wordBox.y = pen->y;
            plotString(p, w, glyphs, &wordBox);
            if (flags & FE_WRAP &&  // want wrap
                    pen->x + wordBox.w > r->w &&  // but wouldn't fit on this line
                    wordBox.w <= r->w) {  // but would fit on next
                wordBox.x = pen->x = 0;
                pen->y += m_cur.lineHeight;
                wordBox.y = pen->y;
            }
            if (pen->y >= r->h - m_cur.descender)
                return p - str;
            wordBox.y -= m_cur.ascender;
            if (bbox)
                bbox->unionRect(&wordBox);

            // Fits; render it and advance
            if (flags & FE_NOBLIT) {
                for (unsigned int i = 0; glyphs[i]; ++i) {
                    pen->x += glyphs[i]->advanceX;
                    pen->y += glyphs[i]->advanceY;
                }
            } else {
                Pos dst;
                int xOffset = 0;
                if (flags & FE_XCENTER) {
                    for (unsigned int i = 0; glyphs[i]; ++i)
                        xOffset += glyphs[i]->advanceX;
                    xOffset = r->w/2 - (xOffset>>1);
                }
                dst.x = pen->x + r->x + xOffset;
                dst.y = pen->y + r->y;
                g_fb->blitGlyphs(glyphs, &dst, r);
                pen->x = dst.x - r->x - xOffset;
                pen->y = dst.y - r->y;
            }
            p += w;
            len -= w;
        }

        // Word-wrap or hard linefeed, but avoid the two back-to-back.
        if (pen->x >= r->w && !(flags & FE_WRAP))
            return p - str;
        if ((*p == '\n' && !wordWrapped) || pen->x >= r->w) {
            pen->x = 0;
            pen->y += m_cur.lineHeight;
            if (*p == '\n' && len) {
                p++;
                len--;
            } else {
                wordWrapped = true;
            }
            if (pen->y >= r->h - m_cur.descender)
                return p - str;
        } else {
            if (*p == '\n') {
                p++;
                len--;
            }
            wordWrapped = false;
        }
    } while (len > 0);
    return -1;  // TODO
}

