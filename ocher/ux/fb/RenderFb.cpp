/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#include <ctype.h>

#include "clc/support/Debug.h"
#include "clc/support/Logger.h"
#include "clc/os/Stopwatch.h"

#include "ocher/fmt/Layout.h"
#include "ocher/settings/Settings.h"
#include "ocher/ux/Pagination.h"
#include "ocher/ux/fb/FrameBuffer.h"
#include "ocher/ux/fb/RenderFb.h"
#include "ocher/ux/Factory.h"

#define LOG_NAME "ocher.render.fb"
#ifdef DEBUG
#define CPS_STATS
#endif


RenderFb::RenderFb(FrameBuffer* fb) :
    m_fb(fb),
    m_penX(g_settings.marginLeft),
    m_penY(g_settings.marginTop),
    ai(0)
{
}

bool RenderFb::init()
{
    memset(&a[ai], 0, sizeof(a[ai]));
    a[ai].pts = g_settings.fontPoints;
    return true;
}

void RenderFb::pushAttrs()
{
    a[ai+1] = a[ai];
    ai++;
}

void RenderFb::popAttrs()
{
    ai--;
}

int RenderFb::outputWrapped(clc::Buffer* b, unsigned int strOffset, bool doBlit)
{
    unsigned int len = b->size();
    const char* start = b->data();
    const char* p = start;

    ASSERT(strOffset <= len);
    len -= strOffset;
    p += strOffset;

    if (m_penY == g_settings.marginTop) {
        while (*p == '\n') {
            ++p;
            --len;
        }
    }
    if (m_penX == g_settings.marginLeft) {
        if (m_penY >= (int)m_fb->height() - g_settings.marginBottom - m_fe.m_cur.descender) {
            return 0;
        }
    }

    bool wordWrapped = false;
    int width = m_fb->width();
    do {
        // If at start of line, eat spaces
        if (m_penX == g_settings.marginLeft) {
            while (*p != '\n' && isspace(*p)) {
                ++p;
                --len;
            }
        }

        if (*p != '\n') {
            // Where is the next word break?
            unsigned int w = 0;
            while (*(p+w) && !isspace(*(p+w))) {
                ++w;
            }
            if (*(p+w))
                ++w;

            Rect bbox;
            Glyph* glyphs[w+1];
            bbox.x = m_penX;
            bbox.y = m_penY;
            m_fe.plotString(p, w, glyphs, &bbox);
            if (m_penX + bbox.w >= width - g_settings.marginRight &&
                    bbox.w <= width - g_settings.marginRight - g_settings.marginLeft) {
                bbox.x = m_penX = g_settings.marginLeft;
                m_penY += m_fe.m_cur.lineHeight;
                bbox.y = m_penY;
            }
            if (m_penY >= (int)m_fb->height() - g_settings.marginBottom - m_fe.m_cur.descender)
                return p - start;
            bbox.y -= m_fe.m_cur.ascender;
            bbox.h = m_fe.m_cur.lineHeight;
            /* TODO save bounding box + glyphs for selection */

            // Fits; render it and advance
            if (doBlit) {
                Pos pos(m_penX, m_penY);
                g_fb->blitGlyphs(glyphs, &pos);
                m_penX = pos.x;
                m_penY = pos.y;
            } else {
                for (unsigned int i = 0; glyphs[i]; ++i) {
                    Glyph* g = glyphs[i];
                    m_penX += g->advanceX;
                    m_penY += g->advanceY;
                }
            }
            p += w;
            len -= w;
        }

        // Word-wrap or hard linefeed, but avoid the two back-to-back.
        if ((*p == '\n' && !wordWrapped) || m_penX >= width-1 - g_settings.marginRight) {
            m_penX = g_settings.marginLeft;
            m_penY += m_fe.m_cur.lineHeight;
            if (*p == '\n') {
                p++;
                len--;
            } else {
                wordWrapped = true;
            }
            if (m_penY >= (int)m_fb->height() - g_settings.marginBottom - m_fe.m_cur.descender)
                return p - start;
        } else {
            if (*p == '\n') {
                p++;
                len--;
            }
            wordWrapped = false;
        }
    } while (len > 0);
    return -1;  // think of this as "failed to cross page boundary"
}

void RenderFb::applyAttrs()
{
    m_fe.setSize(a[ai].pts);
    m_fe.setBold(a[ai].b);
    m_fe.setUnderline(a[ai].ul);
    m_fe.setItalic(a[ai].em);
    m_fe.apply();
}

int RenderFb::render(Pagination* pagination, unsigned int pageNum, bool doBlit)
{
    clc::Log::info(LOG_NAME, "%s page %u of %u", doBlit?"render":"layout", pageNum, pagination->numPages());
    // TODO reapply font settings before relying on font metrics
    m_penX = g_settings.marginLeft;
    m_penY = g_settings.marginTop + m_fe.m_cur.ascender;
    if (doBlit)
        m_fb->clear();

    unsigned int layoutOffset;
    unsigned int strOffset;
    if (!pageNum) {
        layoutOffset = 0;
        strOffset = 0;
    } else if (! pagination->get(pageNum-1, &layoutOffset, &strOffset)) {
        // Previous page not already paginated?
        // Perhaps at end of book?
        clc::Log::error(LOG_NAME, "page %u not found", pageNum);
        return -1;
    }

    int r = 1;
    const unsigned int N = m_layout.size();
    const char* raw = m_layout.data();
    ASSERT(layoutOffset <= N);

    applyAttrs();
#ifdef CPS_STATS
    clc::Stopwatch sw;
    unsigned int chars = 0;
#endif
    unsigned int i = layoutOffset;
    while (i < N) {
        ASSERT(i+2 <= N);
        uint16_t code = *(uint16_t*)(raw+i);
        i += 2;

        unsigned int opType = (code>>12)&0xf;
        unsigned int op = (code>>8)&0xf;
        unsigned int arg = code & 0xff;
        switch (opType) {
            case Layout::OpPushTextAttr:
                clc::Log::trace(LOG_NAME, "OpPushTextAttr");
                switch (op) {
                    case Layout::AttrBold:
                        pushAttrs();
                        clc::Log::trace(LOG_NAME, "font push bold");
                        a[ai].b = 1;
                        m_fe.setBold(1);
                        break;
                    case Layout::AttrUnderline:
                        pushAttrs();
                        clc::Log::trace(LOG_NAME, "font push underline");
                        a[ai].ul = 1;
                        m_fe.setUnderline(1);
                        break;
                    case Layout::AttrItalics:
                        pushAttrs();
                        clc::Log::trace(LOG_NAME, "font push italics");
                        a[ai].em = 1;
                        m_fe.setItalic(1);
                        break;
                    case Layout::AttrSizeRel:
                        pushAttrs();
                        clc::Log::trace(LOG_NAME, "font push rel %d", (int)arg);
                        a[ai].pts += (int)arg;
                        m_fe.setSize(a[ai].pts);
                        break;
                    case Layout::AttrSizeAbs:
                        pushAttrs();
                        clc::Log::trace(LOG_NAME, "font push abs %d", (int)arg);
                        a[ai].pts = (int)arg;
                        m_fe.setSize(a[ai].pts);
                        break;
                    default:
                        clc::Log::error(LOG_NAME, "unknown OpPushTextAttr");
                        ASSERT(0);
                        break;
                }
                break;
            case Layout::OpPushLineAttr:
                clc::Log::trace(LOG_NAME, "OpPushLineAttr");
                switch (op) {
                    case Layout::LineJustifyLeft:
                        break;
                    case Layout::LineJustifyCenter:
                        break;
                    case Layout::LineJustifyFull:
                        break;
                    case Layout::LineJustifyRight:
                        break;
                    default:
                        clc::Log::error(LOG_NAME, "unknown OpPushLineAttr");
                        ASSERT(0);
                        break;
                }
                break;
            case Layout::OpCmd:
                switch (op) {
                    case Layout::CmdPopAttr:
                        clc::Log::trace(LOG_NAME, "OpCmd CmdPopAttr");
                        if (arg == 0)
                            arg = 1;
                        while (arg--)
                            popAttrs();
                        clc::Log::trace(LOG_NAME, "font pop (%d pts%s%s)", a[ai].pts, a[ai].b?" bold":"", a[ai].em?" italics":"");
                        applyAttrs();
                        break;
                    case Layout::CmdOutputStr: {
                        m_fe.apply();
                        clc::Log::trace(LOG_NAME, "OpCmd CmdOutputStr");
                        ASSERT(i + sizeof(clc::Buffer*) <= N);
                        clc::Buffer* str = *(clc::Buffer**)(raw+i);
                        ASSERT(strOffset <= str->size());
#ifdef CPS_STATS
                        chars += str->length();  // miscounts UTF8...
#endif
                        clc::Log::debug(LOG_NAME, "output (%d pts%s%s) %u bytes", a[ai].pts, a[ai].b?" bold":"",
                                a[ai].em?" italics":"", (unsigned int)str->length());
                        int breakOffset = outputWrapped(str, strOffset, doBlit);
                        strOffset = 0;
                        if (breakOffset >= 0) {
                            pagination->set(pageNum, i-2, breakOffset);
                            r = 0;
                            goto done;
                        }
                        i += sizeof(clc::Buffer*);
                        break;
                    }
                    case Layout::CmdForcePage:
                        clc::Log::trace(LOG_NAME, "OpCmd CmdForcePage");
                        pagination->set(pageNum, i, 0);
                        r = 0;
                        goto done;
                    default:
                        clc::Log::error(LOG_NAME, "unknown OpCmd");
                        ASSERT(0);
                        break;
                }
                break;
            case Layout::OpSpacing:
                break;
            case Layout::OpImage:
                break;
            default:
                clc::Log::error(LOG_NAME, "unknown op type");
                ASSERT(0);
                break;
        };
    }
done:
#ifdef CPS_STATS
    uint64_t usec = sw.elapsedUSec();
    static unsigned int totalChars = 0;
    static uint64_t totalUSec = 0;
    totalChars += chars;
    totalUSec += usec;
    unsigned int ms = usec/1000;
    if (ms == 0)
        ms = 1;
    clc::Log::debug(LOG_NAME, "page %u %s; %u chars in %u ms, %u cps, avg %u cps", pageNum,
            r == 0 ? "break" : "done",
            chars, ms, (unsigned int)(chars*1000/ms),
            (unsigned int)(totalChars*1000/((totalUSec<1000?1000:totalUSec)/1000)));
#endif
    return r;
}
