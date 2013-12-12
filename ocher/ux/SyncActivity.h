/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#ifndef OCHER_UX_SYNCACTIVITY_H
#define OCHER_UX_SYNCACTIVITY_H

#include "ocher/ux/Activity.h"
#include "ocher/ux/fb/Widgets.h"


class SyncActivityWork;

class SyncActivity : public Window
{
public:
    SyncActivity(Controller* c);

    void draw();

protected:
    void onAttached();
    void onDetached();

    Controller* m_controller;
    SyncActivityWork* m_work;
    Spinner m_spinner;
};

#endif

