/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_UX_FD_ACTIVITY_H
#define OCHER_UX_FD_ACTIVITY_H

#include "ux/Activity.h"

class UxControllerFd;

/** Specialization of Activity for file descriptors
 */
class ActivityFd : public Activity {
public:
    ActivityFd(UxControllerFd *uxController) :
        m_uxController(uxController),
        m_in(0), // TODO
        m_out(1) // TODO
    {
    }

    virtual ~ActivityFd() = default;

protected:
    virtual void onAttached() = 0;
    virtual void onDetached() = 0;

    UxControllerFd *m_uxController;

    int m_in;
    int m_out;
};

#endif
