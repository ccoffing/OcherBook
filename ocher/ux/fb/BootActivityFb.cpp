/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ux/fb/BootActivityFb.h"

#include "Container.h"
#include "ux/fb/FontEngine.h"
#include "ux/fb/UxControllerFb.h"
#include "util/Logger.h"

#include <unistd.h>

#define LOG_NAME "ocher.ux.Boot"


BootActivityFb::BootActivityFb(UxControllerFb* c) :
    ActivityFb(c)
{
    float ratio = 1.6;
    int w = m_fb->xres();
    int h = m_fb->yres();

    // Calc nice percentage widths, then calc height proportionally, then center vertically.
    apps[0].x = w / 10;
    apps[0].w = w / 10 * 3;
    apps[0].h = apps[0].w * ratio;
    apps[0].y = (h - apps[0].h) / 2;
    apps[1].x = apps[0].x + w / 2;
    apps[1].w = apps[0].w;
    apps[1].h = apps[0].h;
    apps[1].y = apps[0].y;
}

void BootActivityFb::highlight(int i)
{
    Log::debug(LOG_NAME, "highlight %d", i);
    Rect r = apps[i];

    for (int n = 0; n < 4; ++n) {
        r.inset(-1);
        m_fb->rect(&r);
    }
}

EventDisposition BootActivityFb::evtMouse(const struct OcherMouseEvent* evt)
{
    if (evt->subtype == OEVT_MOUSE1_UP) {
        Pos pos(evt->x, evt->y);
        if (apps[0].contains(pos)) {
            highlight(0);
            m_uxController->setNextActivity(Activity::Type::Sync);
        } else if (apps[1].contains(pos)) {
            highlight(1);
            exit(1);
        }
    }
    return EventDisposition::Handled;
}

void BootActivityFb::drawContent(const Rect* rect)
{
    Log::debug(LOG_NAME, "draw");

    m_fb->setFg(0xff, 0xff, 0xff);
    m_fb->fillRect(rect);

    m_fb->setFg(0, 0, 0);
    auto fe = m_uxController->getFontEngine();
    FontFace face;
    face.points = 16;
    face.italic = true;
    const auto fc = fe->context(face);

    Pos pos;
    for (int i = 0; i < 2; ++i) {
        Rect r = apps[i];
        m_fb->rect(&r);
        r.inset(-1);
        m_fb->roundRect(&r, 1);
        r.inset(2);

        const char* label = i == 0 ? "OcherBook" : "Kobo" /* or "Nickel" ? */;
        pos.x = 0;
        pos.y = r.h / 2;
        fe->renderString(fc, label, strlen(label), &pos, &r, FE_XCENTER);
    }
    m_fb->update(nullptr);
}
