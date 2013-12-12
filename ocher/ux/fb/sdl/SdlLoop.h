/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#ifndef OCHER_SDL_LOOP_H
#define OCHER_SDL_LOOP_H

#include "clc/data/List.h"
#include "clc/os/Lock.h"
#include "clc/os/Monitor.h"
#include "clc/os/Thread.h"
#include "ocher/ux/Event.h"


/**
 * Thread to initialize SDL graphics and gather SDL events.  Note that SDL must do these two things
 * on the same thread, but the main thread of OcherBook knows nothing of SDL.  Therefore SDL runs on
 * its own thread, and feeds events back to the main thread via libev to be fired there.
 */
class SdlLoop : public clc::Thread
{
public:
	SdlLoop();
	~SdlLoop();

	void setEventLoop(EventLoop* loop);

	void start(clc::Monitor* startupMonitor);
	void stop();

	SDL_Surface* getScreen() { return m_screen; }

protected:
	SDL_Surface* init();
	void run();

	clc::Lock m_evtLock;
	clc::List m_evtQ;
	int m_pipe[2];
	struct ev_io m_evtWatcher;
	static void fireEventsCb(struct ev_loop*, ev_io* watcher, int);
	void fireEvents();

	EventLoop* m_loop;
	clc::Monitor* m_startupMonitor;
	SDL_Surface* m_screen;
};

#endif

