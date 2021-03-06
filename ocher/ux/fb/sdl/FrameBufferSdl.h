/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef SDL_FB_H
#define SDL_FB_H

#include "ux/fb/FrameBuffer.h"
#include "ux/fb/sdl/SdlThread.h"

#include <SDL.h>


class FrameBufferSdl : public FrameBuffer {
public:
    FrameBufferSdl(EventLoop& loop);
    virtual ~FrameBufferSdl();

    bool init() override;

    int xres() override;
    int yres() override;
    int ppi() override;

    void setFg(uint8_t r, uint8_t b, uint8_t g) override;
    void setBg(uint8_t r, uint8_t b, uint8_t g) override;
    void clear() override;
    void fillRect(const Rect* r) override;
    void byLine(const Rect* r, void (*fn)(void* p, size_t n)) override;
    void pset(int x, int y) override;
    void hline(int x1, int y, int x2) override;
    void vline(int x, int y1, int y2) override;
    void blit(const unsigned char* p, int x, int y, int w, int h, const Rect* clip) override;
    int update(const Rect* r, bool full = false) override;

protected:
    EventLoop& m_loop;
    SdlThread m_sdlThread;

    int m_sdl;
    SDL_Surface* m_screen;
    bool m_mustLock;
    uint8_t m_fgColor;
    uint8_t m_bgColor;

    uint8_t getColor(uint8_t r, uint8_t b, uint8_t g);
};

#endif
