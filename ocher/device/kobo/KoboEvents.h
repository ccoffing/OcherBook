/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#ifndef OCHER_KOBO_EVENT_H
#define OCHER_KOBO_EVENT_H

#include <linux/input.h>

#include "ocher/ux/Event.h"


class KoboEvents
{
public:
	KoboEvents();
	~KoboEvents();

	void setEventLoop(EventLoop* loop);

protected:
	int m_buttonFd;
	struct ev_io m_buttonWatcher;
	static void buttonCb(struct ev_loop* loop, ev_io* watcher, int revents);
	void pollButton();

	int m_touchFd;
	struct ev_io m_touchWatcher;
	static void touchCb(struct ev_loop* loop, ev_io* watcher, int revents);
	void pollTouch();

	EventLoop* m_loop;
	OcherMouseEvent m_evt;
};

#endif

