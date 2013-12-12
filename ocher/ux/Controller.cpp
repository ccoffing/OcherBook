/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#include <unistd.h>
#include "clc/support/Debug.h"
#include "clc/support/Logger.h"
#include "ocher/device/Device.h"
#include "ocher/ux/Controller.h"
#include "ocher/ux/Factory.h"
#include "ocher/ux/PowerSaver.h"

#define LOG_NAME "ocher.controller"


Controller::Controller() :
    m_activity(ACTIVITY_SYNC),
    m_activityPanel(0),
    m_homeActivity(this),
    m_libraryActivity(this),
    m_readActivity(this),
    m_settingsActivity(this),
    m_syncActivity(this),
    m_bootActivity(this)
{
    // TODO:  replace with IOC

    g_fb->setEventLoop(g_loop);

    g_device->fs.dirChanged.Connect(this, &Controller::onDirChanged);

    m_screen.setEventLoop(g_loop);
    m_screen.setFrameBuffer(g_fb);

    m_powerSaver = new PowerSaver();
    m_powerSaver->setEventLoop(g_loop);
    m_powerSaver->wantToSleep.Connect(this, &Controller::onWantToSleep);

    g_loop->appEvent.Connect(this, &Controller::onAppEvent);
}

Controller::~Controller()
{
    g_device->fs.dirChanged.Disconnect(this, &Controller::onDirChanged);

    m_powerSaver->wantToSleep.Disconnect(this, &Controller::onWantToSleep);
    delete m_powerSaver;

    g_loop->appEvent.Disconnect(this, &Controller::onAppEvent);
}

void Controller::onDirChanged(const char* dir, const char* file)
{
    clc::Log::info(LOG_NAME, "onDirChanged %s %s", dir, file);
    // TODO
}

void Controller::detachCurrent()
{
    if (m_activityPanel) {
        m_screen.removeChild(m_activityPanel);
        m_activityPanel->onDetached();
    }
}

void Controller::attachCurrent()
{
    if (m_activityPanel) {
        m_screen.addChild(m_activityPanel);
        m_activityPanel->onAttached();
    }
}

void Controller::onWantToSleep()
{
    clc::Log::info(LOG_NAME, "onWantToSleep");

    detachCurrent();

    m_screen.addChild(m_powerSaver);
    m_powerSaver->onAttached();
    ::sleep(1); // TODO seems hackish but sync doesn't wait long enough!
    m_powerSaver->sleep();
    //g_loop->flush(now);
    m_screen.removeChild(m_powerSaver);
    m_powerSaver->onDetached();

    attachCurrent();
}

void Controller::onAppEvent(struct OcherAppEvent* evt)
{
    if (evt->subtype == OEVT_APP_CLOSE) {
        detachCurrent();
        g_loop->stop();
    }
}

void Controller::setNextActivity(Activity a)
{
    clc::Log::info(LOG_NAME, "next activity: %d", a);
    detachCurrent();

    switch (a) {
        case ACTIVITY_BOOT:
            m_activityPanel = &m_bootActivity;
            break;
        case ACTIVITY_SYNC:
            m_activityPanel = &m_syncActivity;
            break;
        case ACTIVITY_HOME:
            m_activityPanel = &m_homeActivity;
            break;
        case ACTIVITY_READ:
            m_activityPanel = &m_readActivity;
            break;
        case ACTIVITY_LIBRARY:
            m_activityPanel = &m_libraryActivity;
            break;
        case ACTIVITY_SETTINGS:
            m_activityPanel = &m_settingsActivity;
            break;
        default:
            ASSERT(0);
    }

    attachCurrent();
    g_fb->needFull();
}

void Controller::run(Activity activity)
{
    setNextActivity(activity);
    g_loop->run();

    // TODO: sync state out
}
