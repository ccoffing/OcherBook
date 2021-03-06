/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ux/fb/SystemBar.h"

#include "ux/fb/BatteryIcon.h"
#include "ux/fb/ClockIcon.h"
#include "ux/fb/UxControllerFb.h"

#include "Container.h"
#include "settings/Settings.h"


SystemBar::SystemBar(UxControllerFb* uxController, Battery& battery) :
    m_sep(false),
    m_uxController(uxController)
{
    const auto fc = m_screen->fe->context();

    setRect(0, 0, m_fb->xres() - 1, 1);

    m_flags |= WIDGET_BORDERLESS;

    auto menu = make_unique<Menu>(0, 0);
    menu->addItem("Home", [&](){
            m_uxController->setNextActivity(Activity::Type::Home);
        });
    menu->addItem("Library", [&](){
            m_uxController->setNextActivity(Activity::Type::Library);
        });
    menu->addItem("Settings", [&](){
            m_uxController->setNextActivity(Activity::Type::Settings);
        });
    addChild(std::move(menu));

    // TODO h-center
    addChild(make_unique<ClockIcon>(m_fb->xres() / 2, fc.bearingY()));

    // TODO v-center
    auto batteryIcon = make_unique<BatteryIcon>(0, 0, battery);
    batteryIcon->setPos({m_fb->xres() - 1 - batteryIcon->rect().w, 0});
    addChild(std::move(batteryIcon));

    resize();
}


void SystemBar::drawContent(const Rect* r)
{
    if (m_sep) {
        m_fb->setFg(0, 0, 0);
        m_fb->hline(r->x, r->y + r->h - 1, r->x + r->w - 1);
    }
    if (m_title.length()) {
        /* TODO  need factories...
           FontEngine fe;
           fe.setSize(10);
           fe.setItalic(0);
           fe.setBold(0);
           fe.apply();
           Pos pos;
           pos.x = 0; pos.y = fe.m_cur.ascender + 2;
           fe.renderString(m_title.c_str(), m_title.length(), &pos, &m_fb->bbox, FE_XCENTER);
         */
    }
}
