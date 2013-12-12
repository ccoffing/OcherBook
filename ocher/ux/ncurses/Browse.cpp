/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#include "ocher/ux/Factory.h"
#include "ocher/ux/ncurses/Browse.h"
#include "ocher/shelf/Meta.h"


BrowseCurses::BrowseCurses()
{
}

bool BrowseCurses::init(CDKSCREEN* screen)
{
    m_screen = screen;
    return true;
}

Meta* BrowseCurses::browse(clc::List* meta)
{
    const unsigned int nItems = meta->size();
    char* items[nItems];

    for (unsigned int i = 0; i < nItems; ++i) {
        Meta* m = (Meta*)meta->get(i);
        items[i] = m->relPath.c_str();
    }

    CDKSCROLL* scroll = newCDKScroll(m_screen, LEFT, TOP, RIGHT, 0, 0, "Select a book...",
            items, nItems, 0, 0, 1, 1);
    int r = activateCDKScroll(scroll, NULL);
    destroyCDKScroll(scroll);
    if (r == -1) {
        return (Meta*)0;
    } else {
        return (Meta*)meta->get(r);
    }
}

void BrowseCurses::read(Meta* meta)
{
    Renderer* renderer = uiFactory->getRenderer();
    for (int pageNum = 0; ; ) {
        int atEnd = renderer->render(&meta->m_pagination, pageNum, true);

        int key = getch();
        if (key == 'p' || key == 'b') {
            if (pageNum > 0)
                pageNum--;
        } else if (key == 'q' || atEnd) {
            break;
        } else {
            pageNum++;
        }
    }
}

