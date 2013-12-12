/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#include <ev.h>
#include "clc/support/Logger.h"
#include "ocher/device/Device.h"
#include "ocher/settings/Settings.h"
#include "ocher/ux/Factory.h"
#include "ocher/ux/PowerSaver.h"
#include "ocher/ux/fb/FontEngine.h"

#define LOG_NAME "ocher.ux.power"


ev_timer timeout_watcher;

void PowerSaver::timeoutCb(EV_P_ ev_timer* timer, int revents)
{
	clc::Log::debug(LOG_NAME, "timeout");
	((PowerSaver*)timer->data)->timeout();
}

void PowerSaver::timeout()
{
	wantToSleep();
}

PowerSaver::PowerSaver() :
	m_loop(0),
	m_seconds(15*60), // TODO settings
	m_device(g_device)
{
}

void PowerSaver::setEventLoop(EventLoop* loop)
{
	m_loop = loop;

	m_loop->mouseEvent.Connect(this, &PowerSaver::onMouseEvent);
	m_loop->keyEvent.Connect(this, &PowerSaver::onKeyEvent);
	m_loop->appEvent.Connect(this, &PowerSaver::onAppEvent);
	m_loop->deviceEvent.Connect(this, &PowerSaver::onDeviceEvent);

	resetTimeout();
}

void PowerSaver::setDevice(Device* device)
{
	m_device = device;
}

void PowerSaver::setTimeout(unsigned int seconds)
{
	m_seconds = seconds;
	resetTimeout();
}

void PowerSaver::resetTimeout()
{
	ev_timer_init(&m_timer, timeoutCb, m_seconds, 0.);
	ev_timer_start(m_loop->evLoop, &m_timer);
	m_timer.data = this;
}

void PowerSaver::onMouseEvent(struct OcherMouseEvent* evt)
{
	resetTimeout();
}

void PowerSaver::onKeyEvent(struct OcherKeyEvent* evt)
{
	if (evt->key == OEVTK_POWER && evt->subtype == OEVT_KEY_UP)
		timeout();
	else
		resetTimeout();
}

void PowerSaver::onAppEvent(struct OcherAppEvent* evt)
{
}

void PowerSaver::onDeviceEvent(struct OcherDeviceEvent* evt)
{
}


void PowerSaver::sleep()
{
	m_device->sleep();
}

void PowerSaver::onAttached()
{
	maximize();

	g_fb->setFg(0xff, 0xff, 0xff);
	g_fb->fillRect(&m_rect);
	g_fb->setFg(0, 0, 0);

	FontEngine fe;
	fe.setSize(18);
	fe.setItalic(1);
	fe.apply();

	Pos p;
	p.x = 0;
	p.y = m_rect.h/2;
	fe.renderString("Sleeping", 8, &p, &m_rect, FE_XCENTER);

	g_fb->update(&m_rect, true);  // Full refresh to clear page remnants
	g_fb->sync();
}

void PowerSaver::onDetached()
{
}
