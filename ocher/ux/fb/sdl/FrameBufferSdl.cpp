/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ocher/ux/fb/sdl/FrameBufferSdl.h"
#include "ocher/util/Bitops.h"
#include "ocher/util/Logger.h"

#include "SDL/SDL.h"

#include <exception>

#define LOG_NAME "ocher.sdl"

// Ocher's Rect is chosen to match SDL's.
#define TO_SDL_RECT(r) ((SDL_Rect *)(r))


FrameBufferSdl::FrameBufferSdl() :
    m_sdl(0),
    m_screen(0)
{
}

FrameBufferSdl::~FrameBufferSdl()
{
    m_sdlThread.stop();
    if (m_sdl)
        SDL_Quit();
}

void FrameBufferSdl::inject(EventLoop *loop)
{
    m_sdlThread.setEventLoop(loop);
}

bool FrameBufferSdl::init()
{
    std::promise<SDL_Surface*> screenPromise;
    std::future<SDL_Surface*> screenFuture = screenPromise.get_future();

    m_sdlThread.start(screenPromise);

    m_screen = screenFuture.get();
    if (!m_screen) {
        return false;
    }

    SDL_Color colors[256];
    for (unsigned int i = 0; i < 256; ++i) {
        colors[i].r = colors[i].g = colors[i].b = i;
    }

    SDL_SetPalette(m_screen, SDL_LOGPAL | SDL_PHYSPAL, colors, 0, 256);
    m_mustLock = SDL_MUSTLOCK(m_screen);
    setBg(0xff, 0xff, 0xff);
    clear();
    m_sdl = 1;

    bbox.x = bbox.y = 0;
    bbox.w = width();
    bbox.h = height();
    Log::info(LOG_NAME, "fb %d %d %d %d", bbox.x, bbox.y, bbox.w, bbox.h);

    return true;
}

uint8_t FrameBufferSdl::getColor(uint8_t r, uint8_t b, uint8_t g)
{
    // return (((uint16_t)r*61)+((uint16_t)g*174)+((uint16_t)b*21))>>8;
    g = ((uint32_t)r + (uint32_t)b + (uint32_t)g) / 3;
    return SDL_MapRGB(m_screen->format, g, g, g);
}

void FrameBufferSdl::setFg(uint8_t r, uint8_t b, uint8_t g)
{
    m_fgColor = getColor(r, b, g);
}

void FrameBufferSdl::setBg(uint8_t r, uint8_t b, uint8_t g)
{
    m_bgColor = getColor(r, b, g);
}

inline unsigned int FrameBufferSdl::height()
{
    return m_screen->h;
}

inline unsigned int FrameBufferSdl::width()
{
    return m_screen->w;
}

unsigned int FrameBufferSdl::dpi()
{
    return 120;  // TODO
}

void FrameBufferSdl::clear()
{
    Log::debug(LOG_NAME, "clear");

    SDL_FillRect(m_screen, NULL, m_bgColor);
    SDL_UpdateRect(m_screen, 0, 0, 0, 0);
}

void FrameBufferSdl::fillRect(Rect *_r)
{
    SDL_Rect *r = TO_SDL_RECT(_r);

    SDL_FillRect(m_screen, r, m_fgColor);
}

void FrameBufferSdl::byLine(Rect *r, void (*fn)(void *p, size_t n))
{
    int y2 = r->y + r->h;

    for (int y = r->y; y < y2; ++y) {
        fn(((unsigned char *)m_screen->pixels) + y * m_screen->pitch + r->x, r->w);
    }
}

inline void FrameBufferSdl::pset(int x, int y)
{
    *(((unsigned char *)m_screen->pixels) + y * m_screen->pitch + x) = m_fgColor;
}

void FrameBufferSdl::hline(int x1, int y, int x2)
{
    // TODO
    line(x1, y, x2, y);
}

void FrameBufferSdl::vline(int x, int y1, int y2)
{
    // TODO
    line(x, y1, x, y2);
}

void FrameBufferSdl::blit(const unsigned char *p, int x, int y, int w, int h, const Rect *userClip)
{
    Log::trace(LOG_NAME, "blit %d %d %d %d", x, y, w, h);
    int rectWidth = w;
    Rect clip;

    if (userClip) {
        clip = *userClip;
    } else {
        clip.x = clip.y = 0;
        clip.w = width();
        clip.h = height();
    }

    const int maxX = clip.x + clip.w - 1;
    const int minX = clip.x;
    if (x + w >= maxX) {
        if (x >= maxX)
            return;
        w = maxX - x;
    }
    if (x < minX) {
        // TODO
    }
    const int maxY = clip.y + clip.h - 1;
    const int minY = clip.y;
    if (y + h >= maxY) {
        if (y >= maxY)
            return;
        h = maxY - y;
    }
    if (y < minY) {
        // TODO
    }

    if (m_mustLock) {
        if (SDL_LockSurface(m_screen) < 0) {
            return;
        }
    }
    Log::trace(LOG_NAME, "blit clipped %d %d %d %d", x, y, w, h);
    unsigned char *dst = ((unsigned char *)m_screen->pixels) + y * m_screen->pitch + x;
    for (int i = 0; i < h; ++i) {
        memAnd(dst, p, w);
        dst += m_screen->pitch;
        p += rectWidth;
    }
    if (m_mustLock) {
        SDL_UnlockSurface(m_screen);
    }
}

int FrameBufferSdl::update(Rect *r, bool /*full*/)
{
    Rect _r;

    if (!r) {
        _r.x = _r.y = 0;
        _r.w = width();
        _r.h = height();
        r = &_r;
    }
    Log::debug(LOG_NAME, "update %d %d %u %u", r->x, r->y, r->w, r->h);
    SDL_UpdateRects(m_screen, 1, (SDL_Rect *)r);
    return 0;
}
