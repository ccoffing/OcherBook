/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#ifndef OCHER_UX_LIBRARYACTIVITY_H
#define OCHER_UX_LIBRARYACTIVITY_H

#include "ocher/ux/Activity.h"
#include "clc/data/List.h"

class Meta;

class LibraryActivity : public Window
{
public:
    LibraryActivity(Controller* c);
    ~LibraryActivity();

    void draw();

    int evtKey(struct OcherKeyEvent*);
    int evtMouse(struct OcherMouseEvent*);

protected:
    void onAttached();
    void onDetached();

    Controller* m_controller;
    UiBits& m_ui;
    const clc::List* m_library;
#define BOOKS_PER_PAGE 11
    Rect* m_bookRects;
    int itemHeight;
    unsigned int m_booksPerPage;
    unsigned int m_pages;
    unsigned int m_pageNum;
};

#endif
