/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#ifndef OCHER_DEV_POWER_SAVER_H
#define OCHER_DEV_POWER_SAVER_H

#include "Signals/Signal.h"
using namespace Gallant;

#include "ocher/ux/fb/Widgets.h"
#include "ocher/ux/Event.h"

class Device;


class PowerSaver : public Window
{
public:
    PowerSaver();

    void setEventLoop(EventLoop* loop);
    void setDevice(Device* device);
    void setTimeout(unsigned int seconds);

    Signal0<> wantToSleep;

    void sleep();

    void onAttached();
    void onDetached();

protected:
    void resetTimeout();

    static void timeoutCb(EV_P_ ev_timer* w, int revents);
    void timeout();

    void onMouseEvent(struct OcherMouseEvent* evt);
    void onKeyEvent(struct OcherKeyEvent* evt);
    void onAppEvent(struct OcherAppEvent* evt);
    void onDeviceEvent(struct OcherDeviceEvent* evt);

    EventLoop* m_loop;
    ev_timer m_timer;
    unsigned int m_seconds;

    Device* m_device;
};

#endif
