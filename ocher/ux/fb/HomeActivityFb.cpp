/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#include "ocher/Container.h"
#include "ocher/settings/Settings.h"
#include "ocher/shelf/Meta.h"
#include "ocher/ux/fb/UxControllerFb.h"
#include "ocher/ux/fb/FontEngine.h"
#include "ocher/ux/fb/HomeActivityFb.h"

#include "clc/support/Logger.h"

#define LOG_NAME "ocher.ux.Home"


HomeActivityFb::HomeActivityFb(UxControllerFb* c) :
	ActivityFb(c),
	m_fb(g_container.frameBuffer),
	coverRatio(1.6)
{
	maximize();

	int dx = g_container.settings->smallSpace;
	int dy = g_container.settings->smallSpace;

	books[0].x = m_rect.w/15;
	books[0].y = m_rect.h/5;
	books[0].w = m_rect.w/2.8;
	books[0].h = books[0].w*coverRatio;

	books[1].x = books[0].x + books[0].w + dx;
	books[1].y = m_rect.h/6;
	books[1].w = m_rect.w/4;
	books[1].h = books[1].w*coverRatio;

	books[2].x = books[1].x + books[1].w + dx;
	books[2].w = m_rect.w/5;
	books[2].h = books[2].w*coverRatio;
	books[2].y = books[1].y + books[1].h - books[2].h;

	books[3].x = books[1].x;
	books[3].y = books[1].y + books[1].h + dy;
	books[3].w = books[2].w;
	books[3].h = books[2].h;

	books[4] = books[3];
	books[4].x += books[3].w + dx;
	books[4].w -= 2*dx;
	books[4].h = books[4].w*coverRatio;
}

HomeActivityFb::~HomeActivityFb()
{
}

int HomeActivityFb::evtKey(struct OcherKeyEvent*)
{
	return -1;
}

int HomeActivityFb::evtMouse(struct OcherMouseEvent* evt)
{
	if (evt->subtype == OEVT_MOUSE1_UP) {
		Pos pos(evt->x, evt->y);
		const clc::List* metas = m_uxController->ctx.library.getList();
		for (unsigned int i = 0; i < NUM_CLUSTER_BOOKS; i++) {
			Meta* meta = (Meta*)metas->itemAt(i);
			if (!meta) {
				clc::Log::trace(LOG_NAME, "book %d has no meta", i);
				continue;
			}
			if (books[i].contains(&pos)) {
				clc::Log::info(LOG_NAME, "book %d selected %p", i, meta);
				Rect r = books[i];
				r.inset(-2);
				m_fb->roundRect(&r, 3);
				r.inset(-1);
				m_fb->roundRect(&r, 4);
				m_fb->update(&r);
				m_fb->sync();
				m_uxController->ctx.selected = meta;
				m_uxController->setNextActivity(ACTIVITY_READ);
				return -1;
			}
		}
		if (m_browseLabel.contains(&pos)) {
			m_uxController->setNextActivity(ACTIVITY_LIBRARY);
			return -1;
		}
		// TODO: look at shortlist
	}
	return -1;
}

void HomeActivityFb::draw()
{
	clc::Log::debug(LOG_NAME, "draw");

	m_fb->setFg(0xff, 0xff, 0xff);
	m_fb->fillRect(&m_rect);
	m_fb->setFg(0, 0, 0);

	FontEngine fe(m_fb);
	fe.setSize(12);
	fe.apply();
	Rect r;
	Pos pos;
	const clc::List* metas = m_uxController->ctx.library.getList();
	for (unsigned int i = 0; i < NUM_CLUSTER_BOOKS; ++i) {
		r = books[i];
		r.inset(-1);
		m_fb->rect(&r);
		r.inset(-1);
		m_fb->roundRect(&r, 1);
		r.inset(2);

		Meta* meta = (Meta*)metas->itemAt(i);
		uint8_t c = meta ? 0xf0 : 0xd0;
		m_fb->setFg(c, c, c);
		m_fb->fillRect(&r);
		m_fb->setFg(0, 0, 0);
		if (meta) {
			pos.x = 0;
			pos.y = fe.m_cur.ascender;
			r.inset(2);
			fe.renderString(meta->title.c_str(), meta->title.length(), &pos, &r, FE_YCLIP | FE_WRAP);
		}
	}

	fe.setSize(18);
	fe.apply();
	pos.x = 0; pos.y = 100;
	fe.renderString("HOME", 4, &pos, &m_rect, FE_XCENTER);

	// Shortlist
	fe.setSize(14);
	fe.apply();
	pos.x = books[0].x;
	pos.y = books[3].y + books[3].h + fe.m_cur.ascender + g_container.settings->smallSpace;
	fe.renderString("Shortlist", 9, &pos, &m_rect, 0);

	// TODO simplify, abstract into labels
	{
		fe.setSize(12);
		fe.setItalic(1);
		fe.apply();
		Rect lbox;
		lbox.x = 0;
		lbox.y = pos.y;
		Glyph* glyphs[13];
		const char* text = "Browse all...";
		fe.plotString(text, strlen(text), &glyphs[0], &lbox);
		// TODO  right justify against lbox (remove plotString call); get bbox returned
		pos.x = m_rect.w - books[0].x - lbox.w;
		fe.renderString(text, strlen(text), &pos, &m_rect, 0, &m_browseLabel);
		m_browseLabel.inset(g_container.settings->smallSpace);
	}

	pos.y += fe.m_cur.underlinePos + g_container.settings->smallSpace;
	m_fb->hline(books[0].x, pos.y, m_rect.w - books[0].x);
	pos.y++;
	m_fb->hline(books[0].x, pos.y, m_rect.w - books[0].x);

	pos.x = books[0].x;
	pos.y += g_container.settings->smallSpace;

	{
		const clc::List* shortList = m_uxController->ctx.shortList.getList();
		int margin = books[0].x;

		int h = m_rect.y + m_rect.h - pos.y - margin;
		int w = h / coverRatio;
		Rect sl(pos.x, pos.y, w, h);
		while (sl.x + sl.w <= m_rect.w - margin) {
			m_fb->roundRect(&sl, 1);
			sl.inset(-1);
			m_fb->roundRect(&sl, 2);

			sl.x += sl.w + g_container.settings->smallSpace;

			// TODO
			(void)shortList;
		}
	}

#if 0
	m_fb->byLine(&m_fb->bbox, dim);
	Rect popup(25, 200, 550, 400);
	m_fb->rect(&popup);
	popup.inset(1);
	m_fb->rect(&popup);
	popup.inset(1);
	m_fb->setFg(0xff, 0xff, 0xff);
	m_fb->fillRect(&popup);
#endif
}

void HomeActivityFb::onAttached()
{
	clc::Log::info(LOG_NAME, "attached");

	SystemBar& systemBar = m_uxController->m_systemBar;
	addChild(systemBar);
	systemBar.m_sep = false;
	systemBar.m_title = "HOME";
	systemBar.show();

	invalidate();
}

void HomeActivityFb::onDetached()
{
	clc::Log::info(LOG_NAME, "detached");
	SystemBar& systemBar = m_uxController->m_systemBar;
	removeChild(&systemBar);
}
