/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#include "ocher/ocher.h"
#include "ocher/ux/ncurses/FactoryNC.h"


UX_DRIVER_REGISTER(Curses);


UiFactoryCurses::UiFactoryCurses() :
    m_renderer(0)
{
}

bool UiFactoryCurses::init()
{
    m_renderer = new RenderCurses();

    m_scr = initscr();
    m_screen = initCDKScreen(m_scr);
    initCDKColor();
    m_renderer->init(m_scr, m_screen);

    return true;
}

void UiFactoryCurses::deinit()
{
    delete m_renderer;
    m_renderer = 0;
    destroyCDKScreen(m_screen);
    endCDK();
    delwin(m_scr);
}

const char* UiFactoryCurses::getName()
{
    return "ncurses";
}

Renderer* UiFactoryCurses::getRenderer()
{
    return m_renderer;
}
