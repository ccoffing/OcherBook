/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#include <ev.h>
#include "clc/support/Logger.h"

#include "ocher/ux/Event.h"

#define LOG_NAME "ocher.evt"


EventLoop::EventLoop()
{
    evLoop = EV_DEFAULT;
}

EventLoop::~EventLoop()
{
}

int EventLoop::run()
{
    clc::Log::debug(LOG_NAME, "ev_run");
    ev_run(evLoop, 0);
    clc::Log::debug(LOG_NAME, "ev_run done");

    return 0;
}

void EventLoop::stop()
{
    ev_break(evLoop, EVBREAK_ALL);
}

EventWork::EventWork(EventLoop* loop) :
    clc::Thread("EventWork %p", loop->evLoop),
    m_loop(loop)
{
    ev_async_init(&m_async, notifyCb);
    m_async.data = this;
    ev_async_start(m_loop->evLoop, &m_async);
}

EventWork::~EventWork()
{
}

void EventWork::run()
{
    work();
    ev_async_send(m_loop->evLoop, &m_async);
}

void EventWork::notifyCb(EV_P_ ev_async* w, int revents)
{
    EventWork* self = ((EventWork*)w->data);
    self->notify();
}

