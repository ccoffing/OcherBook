/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#include "ocher/Container.h"
#include "ocher/ux/fb/Widgets.h"

#include "clc/support/Logger.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

#define LOG_NAME "ocher.widgets"

static const unsigned int borderWidth = 2;
static const unsigned int roundRadius = 1;

Widget::Widget() :
	m_flags(WIDGET_DIRTY),
	m_fb(g_container.frameBuffer),
	m_parent(0)
{
}

Widget::Widget(int x, int y, unsigned int w, unsigned int h) :
	m_rect(x, y, w, h),
	m_flags(WIDGET_DIRTY),
	m_fb(g_container.frameBuffer),
	m_parent(0)
{
}

Widget::~Widget()
{
	for (unsigned int i = 0; i < m_children.size(); ++i) {
		Widget* w = (Widget*)m_children.get(i);
		if (w->m_flags & WIDGET_OWNED)
			delete w;
	}
}

void Widget::addChild(Widget* child)
{
	ASSERT(! m_children.hasItem(child));
	child->m_flags |= WIDGET_OWNED;
	m_children.add(child);
	if (! (child->m_flags & WIDGET_HIDDEN)) {
		child->invalidate();
	}
}

void Widget::addChild(Widget& child)
{
	ASSERT(! m_children.hasItem(&child));
	m_children.add(&child);
	if (! (child.m_flags & WIDGET_HIDDEN)) {
		child.invalidate();
	}
}

void Widget::removeChild(Widget* child)
{
	child->m_flags &= ~WIDGET_OWNED;
	m_children.removeItem(child);
}

void Widget::invalidate()
{
	m_flags |= WIDGET_DIRTY;
	for (unsigned int i = 0; i < m_children.size(); ++i) {
		Widget* w = (Widget*)m_children.get(i);
		w->invalidate();
	}
}

void Widget::invalidate(Rect* rect)
{
	// TODO
	invalidate();
}

Rect Widget::drawChildren()
{
	Rect drawn;
	const size_t N = m_children.size();
	for (unsigned int i = 0; i < N; ++i) {
		Widget* w = (Widget*)m_children.get(i);
		if (! (w->m_flags & WIDGET_HIDDEN)) {
			if (w->m_flags & WIDGET_DIRTY) {
				w->draw();
				w->m_flags &= ~WIDGET_DIRTY;
				drawn.unionRect(&w->m_rect);
			}
			Rect r = w->drawChildren();
			drawn.unionRect(&r);
		}
	}
	return drawn;
}


Window::Window() :
	Widget(0, 0, 0, 0),
	m_bgColor(0xffffffff),
	m_borderColor(0),
	m_borderWidth(borderWidth),
	m_winflags(0),
	m_title(0)
{
}

Window::Window(int x, int y, unsigned int w, unsigned int h) :
	Widget(x, y, w, h),
	m_bgColor(0xffffffff),
	m_borderColor(0),
	m_borderWidth(borderWidth),
	m_winflags(0),
	m_title(0)
{
}

Window::~Window()
{
	if (m_title)
		free(m_title);
}

void Window::maximize()
{
	m_rect.x = m_rect.y = 0;
	m_rect.w = m_fb->width();
	m_rect.h = m_fb->height();
}

void Window::setTitle(const char* title)
{
	if (m_title)
		free(m_title);
	m_title = strdup(title);
}

void Window::draw()
{
	Rect rect(m_rect);
	drawBorder(&rect);
	drawTitle(&rect);
	drawBg(&rect);
	drawContent(&rect);
	drawChildren();
}

void Window::drawBorder(Rect* rect)
{
	if (m_borderWidth) {
		m_fb->setFg(0, 0, 0);
		m_fb->rect(rect);
		rect->x += m_borderWidth;
		rect->y += m_borderWidth;
		rect->w -= m_borderWidth*2;
		rect->h -= m_borderWidth*2;
	}
}

void Window::drawTitle(Rect* rect)
{
	if (m_winflags & OWF_CLOSE) {
		m_fb->setFg(0, 0, 0);
		m_fb->line(rect->x + rect->w - 12, rect->y + 4, rect->x + rect->w - 4, rect->y + 12);
		m_fb->line(rect->x + rect->w - 4, rect->y + 4, rect->x + rect->w - 12, rect->y + 12);
		rect->y += 12;
		rect->h -= 12;
	}
}

void Window::drawBg(Rect* rect)
{
	m_fb->setFg(0xff, 0xff, 0xff);
	m_fb->fillRect(rect);
}

void Window::drawContent(Rect*)
{
}


Button::Button(int x, int y, unsigned int w, unsigned int h) :
	Widget(x, y, w, h)
{
}

Button::~Button()
{
}

void Button::draw()
{
	Rect rect(m_rect);
	drawBorder(&rect);
	drawLabel(&rect);
	drawChildren();
}

void Button::drawBorder(Rect* rect)
{
	m_fb->roundRect(rect, roundRadius);
}

void Button::drawLabel(Rect* rect)
{
	if (m_label.length()) {
		/* TODO --
		FontEngine fe;
		fe.setSize(14);
		fe.apply();
		Pos pos;
		pos.x = 0; pos.y = m_rect.h / 2;
		fe.renderString(m_label.c_str(), m_label.length(), &pos, &m_rect, FE_XCENTER);
		*/
	}
}

int Button::evtKey(struct OcherKeyEvent*)
{
	return -2;
}

int Button::evtMouse(struct OcherMouseEvent*)
{
	return -2;
}


Spinner::Spinner(EventLoop* loop) :
	m_state(0),
	m_steps(12),
	m_delayMs(200),
	m_loop(loop)
{
}

Spinner::Spinner(int x, int y, unsigned int w, unsigned int h) :
	Widget(x, y, w, h),
	m_state(0),
	m_steps(12),
	m_delayMs(200)
{
}

Spinner::~Spinner()
{
	stop();
}

void Spinner::start()
{
	clc::Log::debug(LOG_NAME ".spinner", "start");
	ev_timer_init(&m_timer, timeoutCb, 0, m_delayMs/1000.0);
	m_timer.data = this;
	ev_timer_start(m_loop->evLoop, &m_timer);
}

void Spinner::timeoutCb(EV_P_ ev_timer* timer, int)
{
	clc::Log::trace(LOG_NAME ".spinner", "timeout");
	Spinner* self = ((Spinner*)timer->data);
	self->m_state++;
	if (self->m_state >= self->m_steps)
		self->m_state = 0;
	self->invalidate();
}

void Spinner::stop()
{
	clc::Log::debug(LOG_NAME ".spinner", "stop");
	ev_timer_stop(m_loop->evLoop, &m_timer);
}

void Spinner::draw()
{
	clc::Log::trace(LOG_NAME ".spinner", "draw");
	// Numbered like a clock-1, but 0 o'clock is 3 o'clock.
	// Age is time since wave passed by.
	// Blackness drops off exponentially.
	double l1 = m_rect.w/2.0*.4;
	double l2 = m_rect.w/2.0;
	for (unsigned int i = 0; i < m_steps; ++i) {
		unsigned int age = (m_steps - i + m_state) % m_steps;
		uint8_t c = 0xff - (0xff >> age);
		double rad = (double)i / (double)m_steps * M_PI * 2;
		int x1 = m_rect.x+m_rect.w/2 + cos(rad) * l1;
		int y1 = m_rect.y+m_rect.h/2 + sin(rad) * l1;
		int x2 = m_rect.x+m_rect.w/2 + cos(rad) * l2;
		int y2 = m_rect.y+m_rect.h/2 + sin(rad) * l2;
		m_fb->setFg(c, c, c);
		m_fb->line(x1, y1, x2, y2);
		clc::Log::trace(LOG_NAME ".spinner", "%d,%d %d,%d %02x", x1, y1, x2, y2, c);
	}
}



void Icon::draw()
{
	m_fb->blit(bmp->bmp, m_rect.x, m_rect.y, bmp->w, bmp->h);
#if 0
	if (! m_isActive) {
		m_fb->byLine(&m_rect, lighten);
	}
#endif
}


FbScreen::FbScreen()
{
}

FbScreen::~FbScreen()
{
}

void FbScreen::setEventLoop(EventLoop* loop)
{
	clc::Log::info(LOG_NAME ".screen", "setEventLoop");
	m_loop = loop;

	m_loop->mouseEvent.Connect(this, &FbScreen::onMouseEvent);
	m_loop->keyEvent.Connect(this, &FbScreen::onKeyEvent);

	ev_timer_init(&m_timer, timeoutCb, 0.25, 0.25);
	m_timer.data = this;
	ev_timer_start(m_loop->evLoop, &m_timer);

	ev_prepare_init(&m_evPrepare, readyToIdle);
	m_evPrepare.data = this;
	ev_prepare_start(m_loop->evLoop, &m_evPrepare);

	ev_check_init(&m_evCheck, waking);
	m_evCheck.data = this;
	ev_check_start(m_loop->evLoop, &m_evCheck);
}

void FbScreen::setFrameBuffer(FrameBuffer* fb)
{
	m_fb = fb;

	m_rect.x = 0;
	m_rect.y = 0;
	m_rect.w = m_fb->width();
	m_rect.h = m_fb->height();
}

void FbScreen::addChild(Window* child)
{
	Widget *widget = dynamic_cast<Widget*>(child);
	ASSERT(! m_children.hasItem(widget));
	widget->m_flags |= WIDGET_OWNED;
	m_children.add(widget);
	if (! (widget->m_flags & WIDGET_HIDDEN)) {
		widget->invalidate();
	}
}

#if 0
void FbScreen::addChild(Widget& child)
{
	ASSERT(! m_children.hasItem(&child));
	m_children.add(&child);
	if (! (child.m_flags & WIDGET_HIDDEN)) {
		child.invalidate();
	}
}
#endif

void FbScreen::removeChild(Window* child)
{
	Widget *widget = dynamic_cast<Widget*>(child);
	widget->m_flags &= ~WIDGET_OWNED;
	m_children.removeItem(widget);
}

void FbScreen::update()
{
	clc::Log::trace(LOG_NAME ".screen", "update");

	/* This is good enough for now.  Future things:
	 *  - support overlapping children (z order)
	 *  - draw only invalid rects (not entire widget)
	 */
	Rect drawn;
	const size_t N = m_children.size();
	for (unsigned int i = 0; i < N; ++i) {
		Widget* w = (Widget*)m_children.get(i);
		if (! (w->m_flags & WIDGET_HIDDEN)) {
			if (w->m_flags & WIDGET_DIRTY) {
				w->draw();
				w->m_flags &= ~WIDGET_DIRTY;
				drawn.unionRect(&w->m_rect);
			}
			Rect r = w->drawChildren();
			drawn.unionRect(&r);
		}
	}

	if (drawn.valid()) {
		m_fb->update(&drawn);
	}
}

/* Theory of interface with libev:
 *
 * - When event loop wakes up to handle new event(s), start a repeating timer.
 * - Purpose of repeating timer is to ensure screen gets periodic updates, even when events continue
 *   to stream in non-stop.
 * - When even loop goes idle, do one last (perhaps the only) update, and cancel the timer.
 */

void FbScreen::timeoutCb(EV_P_ ev_timer* timer, int)
{
	clc::Log::trace(LOG_NAME ".screen", "timeout");
	((FbScreen*)timer->data)->update();
}

void FbScreen::readyToIdle(EV_P_ ev_prepare* p, int)
{
	clc::Log::trace(LOG_NAME ".screen", "readyToIdle");
	FbScreen* self = (FbScreen*)p->data;
	self->update();
	ev_timer_stop(loop, &self->m_timer);
}

void FbScreen::waking(EV_P_ ev_check* c, int)
{
	clc::Log::trace(LOG_NAME ".screen", "waking");
	FbScreen* self = (FbScreen*)c->data;
	ev_timer_start(loop, &self->m_timer);
}

void FbScreen::onMouseEvent(struct OcherMouseEvent* evt)
{
	/* TODO find the right child */
	for (unsigned int i = 0; i < m_children.size(); ++i) {
		int r = ((Widget*)m_children.get(i))->evtMouse(evt);
		if (r != -2)
			break;
	}
}

void FbScreen::onKeyEvent(struct OcherKeyEvent* evt)
{
	/* TODO find the right child */
	for (unsigned int i = 0; i < m_children.size(); ++i) {
		int r = ((Widget*)m_children.get(i))->evtKey(evt);
		if (r != -2)
			break;
	}
}
