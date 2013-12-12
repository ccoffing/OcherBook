/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#ifndef OCHER_UX_FACTORY_FB_H
#define OCHER_UX_FACTORY_FB_H

#include "ocher/ux/Factory.h"
#include "ocher/ux/fb/FreeType.h"
#include "ocher/ux/fb/RenderFb.h"


class UiFactoryFb : public UiFactory
{
public:
    UiFactoryFb();
    virtual ~UiFactoryFb() {}

    bool init();
    void deinit();

    Renderer* getRenderer() { return m_render; }
    FrameBuffer* getFrameBuffer() { return m_fb; }
    FreeType* getFontEngine() { return m_ft; }

protected:
    FrameBuffer* m_fb;
    FreeType* m_ft;
    RenderFb* m_render;
};

#endif

