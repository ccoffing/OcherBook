/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_FB_RENDER_H
#define OCHER_FB_RENDER_H

#include "ux/Renderer.h"
#include "ux/fb/FontEngine.h"

#include <string>

class FrameBuffer;
class Pagination;
class Settings;


class RendererFb : public Renderer {
public:
    RendererFb(FrameBuffer* fb, FontEngine* fe);

    bool init() override;

    int render(Pagination* pagination, unsigned int pageNum, bool doBlit) override;

protected:
    int outputWrapped(std::string* b, unsigned int strOffset, bool doBlit);
    void applyAttrs();
    void pushAttrs();
    void popAttrs();

    FrameBuffer* m_fb;
    FontEngine* m_fe;
    FontContext m_fc;
    Settings& m_settings;
    int m_penX;
    int m_penY;
    Attrs a[10];
    int ai;
};

#endif
