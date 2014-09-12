/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#include "ocher/resources/Bitmaps.h"
#include "ocher/settings/Settings.h"
#include "ocher/shelf/Meta.h"
#include "ocher/ux/fb/FontEngine.h"
#include "ocher/ux/fb/LibraryActivityFb.h"
#include "ocher/ux/fb/UxControllerFb.h"
#include "ocher/ux/fb/Widgets.h"
#include "ocher/Container.h"

#include "clc/support/Logger.h"

#define LOG_NAME "ocher.ux.Library"


LibraryActivityFb::LibraryActivityFb(UxControllerFb* c) :
	ActivityFb(c),
	m_systemBar(c->m_systemBar),
	m_fb(c->getFrameBuffer()),
	m_settings(g_container.settings),
	m_library(0),
	m_pageNum(0)
{
	maximize();

	// TODO calc this
	m_booksPerPage = BOOKS_PER_PAGE;
	m_bookRects = new Rect[m_booksPerPage];

	m_systemBar.m_sep = false;
	m_systemBar.m_title = "LIBRARY";
	m_systemBar.show();
	addChild(m_systemBar);
	addChild(new Icon(m_settings->medSpace + m_settings->largeSpace,
				m_rect.h - m_settings->medSpace - bmpLeftArrow.h, &bmpLeftArrow));
	addChild(new Icon(m_rect.w - (m_settings->medSpace + m_settings->largeSpace + bmpRightArrow.w),
				m_rect.h - m_settings->medSpace - bmpRightArrow.h, &bmpRightArrow));

}

LibraryActivityFb::~LibraryActivityFb()
{
	delete[] m_bookRects;
}

int LibraryActivityFb::evtKey(struct OcherKeyEvent* evt)
{
	if (evt->subtype == OEVT_KEY_DOWN) {
		if (evt->key == OEVTK_HOME) {
			clc::Log::info(LOG_NAME, "home");
			m_uxController->setNextActivity(ACTIVITY_HOME);
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

int LibraryActivityFb::evtMouse(struct OcherMouseEvent* evt)
{
	if (evt->subtype == OEVT_MOUSE1_UP) {
		Pos pos(evt->x, evt->y);
		for (unsigned int i = 0; i < m_booksPerPage; i++) {
			Meta* meta = (Meta*)m_library->get(i + m_pageNum*m_booksPerPage);
			if (!meta)
				break;
			if (m_bookRects[i].contains(&pos)) {
				m_uxController->ctx.selected = meta;
				m_uxController->setNextActivity(ACTIVITY_READ);
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

void LibraryActivityFb::draw()
{
	clc::Log::debug(LOG_NAME, "draw");

	m_fb->setFg(0xff, 0xff, 0xff);
	m_fb->fillRect(&m_rect);
	m_fb->setFg(0, 0, 0);

	FontEngine fe(m_fb);
	clc::Buffer str;
	Pos pos;
	Rect clip = m_fb->bbox;

	fe.setSize(10);
	fe.apply();
	str.format("PG. %u OF %u", m_pageNum+1, m_pages);
	pos.x = 0;
	pos.y = clip.h - m_settings->smallSpace - fe.m_cur.descender;
	fe.renderString(str.c_str(), str.length(), &pos, &clip, FE_XCENTER);
// TODO        int maxY = pos.y - fe.m_cur.ascender;

	clip.y = m_systemBar.m_rect.y + m_systemBar.m_rect.h + m_settings->medSpace;
	clip.x = m_settings->medSpace;
	clip.w -= m_settings->medSpace*2;
	for (unsigned int i = 0; i < m_booksPerPage; ++i) {
		Meta* meta = (Meta*)m_library->get(i + m_pageNum*m_booksPerPage);
		if (!meta)
			break;

		fe.setSize(12);
		fe.apply();
		pos.x = 0;
		pos.y = m_settings->smallSpace + fe.m_cur.ascender;
		fe.renderString(meta->title.c_str(), meta->title.length(), &pos, &clip, FE_XCLIP);
		pos.x = 0;
		pos.y = m_settings->smallSpace + fe.m_cur.ascender + fe.m_cur.lineHeight;

		fe.setSize(10);
		fe.apply();
		str.format("%u%% read   |   %s", meta->percentRead(), meta->fmtToStr(meta->format));
		fe.renderString(str.c_str(), str.length(), &pos, &clip, FE_XCLIP);

		m_bookRects[i] = clip;
		m_bookRects[i].h = pos.y + fe.m_cur.descender + m_settings->smallSpace;
		clip.y = m_bookRects[i].y + m_bookRects[i].h;

		m_fb->hline(clip.x, clip.y, clip.w);
	}
}

void LibraryActivityFb::onAttached()
{
	clc::Log::info(LOG_NAME, "attached");

	m_library = m_uxController->ctx.library.getList();
	m_pages = (m_library->size() + m_booksPerPage - 1) / m_booksPerPage;
	clc::Log::info(LOG_NAME, "%u books across %u pages", (unsigned)m_library->size(), m_pages);

	m_systemBar.m_sep = false;
	m_systemBar.m_title.clear();
	m_systemBar.show();

	invalidate();
}

void LibraryActivityFb::onDetached()
{
	clc::Log::info(LOG_NAME, "detached");
}
