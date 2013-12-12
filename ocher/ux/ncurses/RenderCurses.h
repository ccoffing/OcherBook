/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#ifndef OCHER_CURSES_RENDER_H
#define OCHER_CURSES_RENDER_H

#include <cdk/cdk.h>

#include "ocher/ux/Renderer.h"

class Pagination;

class RenderCurses : public Renderer
{
public:
    RenderCurses();

    bool init(WINDOW* scr, CDKSCREEN* m_screen);
    int render(Pagination* pagination, unsigned int pageNum, bool doBlit);

    int outputWrapped(clc::Buffer* b, unsigned int strOffset, bool doBlit);

protected:
    WINDOW* m_scr;
    CDKSCREEN* m_screen;
    int m_width;
    int m_height;
    int m_x;
    int m_y;

    void enableUl();
    void disableUl();
    void enableEm();
    void disableEm();

    void pushAttrs();
    void applyAttrs(int i);
    void popAttrs();

    Attrs a[10];
    int ai;
};

#endif
