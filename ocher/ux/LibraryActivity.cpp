/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#include "clc/support/Logger.h"

#include "ocher/resources/Bitmaps.h"
#include "ocher/settings/Settings.h"
#include "ocher/shelf/Meta.h"
#include "ocher/ux/Controller.h"
#include "ocher/ux/Factory.h"
#include "ocher/ux/LibraryActivity.h"
#include "ocher/ux/fb/FontEngine.h"
#include "ocher/ux/fb/Widgets.h"

#define LOG_NAME "ocher.ux.Library"


LibraryActivity::LibraryActivity(Controller* c) :
    m_controller(c),
    m_ui(c->ui),
    m_library(0),
    m_pageNum(0)
{
    maximize();

    // TODO calc this
    m_booksPerPage = BOOKS_PER_PAGE;
    m_bookRects = new Rect[m_booksPerPage];

    m_ui.m_systemBar.m_sep = false;
    m_ui.m_systemBar.m_title = "LIBRARY";
    m_ui.m_systemBar.show();
    addChild(m_ui.m_systemBar);
    addChild(new Icon(g_settings.medSpace + g_settings.largeSpace,
                m_rect.h - g_settings.medSpace - bmpLeftArrow.h, &bmpLeftArrow));
    addChild(new Icon(m_rect.w - (g_settings.medSpace + g_settings.largeSpace + bmpRightArrow.w),
                m_rect.h - g_settings.medSpace - bmpRightArrow.h, &bmpRightArrow));

}

LibraryActivity::~LibraryActivity()
{
    delete[] m_bookRects;
}

int LibraryActivity::evtKey(struct OcherKeyEvent* evt)
{
    if (evt->subtype == OEVT_KEY_DOWN) {
        if (evt->key == OEVTK_HOME) {
            clc::Log::info(LOG_NAME, "home");
            m_controller->setNextActivity(ACTIVITY_HOME);
            return -1;
        } else if (evt->key == OEVTK_LEFT || evt->key == OEVTK_UP || evt->key == OEVTK_PAGEUP) {
            clc::Log::info(LOG_NAME, "back from page %d", m_pageNum);
            if (m_pageNum > 0) {
                m_pageNum--;
                invalidate();
            }
            return -1;
        } else if (evt->key == OEVTK_RIGHT || evt->key == OEVTK_DOWN || evt->key == OEVTK_PAGEDOWN) {
            clc::Log::info(LOG_NAME, "forward from page %d", m_pageNum);
            if (m_pageNum+1 < m_pages) {
                m_pageNum++;
                invalidate();
            }
            return -1;
        }
    }
    return -2;
}

int LibraryActivity::evtMouse(struct OcherMouseEvent* evt)
{
    if (evt->subtype == OEVT_MOUSE1_UP) {
        Pos pos(evt->x, evt->y);
        for (unsigned int i = 0; i < m_booksPerPage; i++) {
            Meta* meta = (Meta*)m_library->get(i + m_pageNum*m_booksPerPage);
            if (!meta)
                break;
            if (m_bookRects[i].contains(&pos)) {
                m_controller->ctx.selected = meta;
                m_controller->setNextActivity(ACTIVITY_READ);
                return -1;
            }
        }
        // TODO buttons
        if (pos.x < 300) {
            clc::Log::info(LOG_NAME, "back from page %d", m_pageNum);
            if (m_pageNum > 0) {
                m_pageNum--;
                invalidate();
            }
        } else {
            clc::Log::info(LOG_NAME, "forward from page %d", m_pageNum);
            if (m_pageNum+1 < m_pages) {
                m_pageNum++;
                invalidate();
            }
        }
    }
    return -1;
}

void LibraryActivity::draw()
{
    clc::Log::debug(LOG_NAME, "draw");

    g_fb->setFg(0xff, 0xff, 0xff);
    g_fb->fillRect(&m_rect);
    g_fb->setFg(0, 0, 0);

    FontEngine fe;
    clc::Buffer str;
    Pos pos;
    Rect clip = g_fb->bbox;

    fe.setSize(10);
    fe.apply();
    str.format("PG. %u OF %u", m_pageNum+1, m_pages);
    pos.x = 0;
    pos.y = clip.h - g_settings.smallSpace - fe.m_cur.descender;
    fe.renderString(str.c_str(), str.length(), &pos, &clip, FE_XCENTER);
// TODO        int maxY = pos.y - fe.m_cur.ascender;

    clip.y = m_ui.m_systemBar.m_rect.y + m_ui.m_systemBar.m_rect.h + g_settings.medSpace;
    clip.x = g_settings.medSpace;
    clip.w -= g_settings.medSpace*2;
    for (unsigned int i = 0; i < m_booksPerPage; ++i) {
        Meta* meta = (Meta*)m_library->get(i + m_pageNum*m_booksPerPage);
        if (!meta)
            break;

        fe.setSize(12);
        fe.apply();
        pos.x = 0;
        pos.y = g_settings.smallSpace + fe.m_cur.ascender;
        fe.renderString(meta->title.c_str(), meta->title.length(), &pos, &clip, FE_XCLIP);
        pos.x = 0;
        pos.y = g_settings.smallSpace + fe.m_cur.ascender + fe.m_cur.lineHeight;

        fe.setSize(10);
        fe.apply();
        str.format("%u%% read   |   %s", meta->percentRead(), meta->fmtToStr(meta->format));
        fe.renderString(str.c_str(), str.length(), &pos, &clip, FE_XCLIP);

        m_bookRects[i] = clip;
        m_bookRects[i].h = pos.y + fe.m_cur.descender + g_settings.smallSpace;
        clip.y = m_bookRects[i].y + m_bookRects[i].h;

        g_fb->hline(clip.x, clip.y, clip.w);
    }
}

void LibraryActivity::onAttached()
{
    clc::Log::info(LOG_NAME, "attached");

    m_library = m_controller->ctx.library.getList();
    m_pages = (m_library->size() + m_booksPerPage - 1) / m_booksPerPage;
    clc::Log::info(LOG_NAME, "%u books across %u pages", (unsigned)m_library->size(), m_pages);

    SystemBar& systemBar = m_controller->ui.m_systemBar;
    systemBar.m_sep = false;
    systemBar.m_title.clear();
    systemBar.show();

    invalidate();
}

void LibraryActivity::onDetached()
{
    clc::Log::info(LOG_NAME, "detached");
}
