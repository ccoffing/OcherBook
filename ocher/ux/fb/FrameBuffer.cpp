/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ux/fb/FrameBuffer.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>


void invert(void* p, size_t n)
{
    auto p1 = (uint8_t*)p;

    while (((ptrdiff_t)p1 & 3) && n) {
        *p1 = ~*p1;
        ++p1;
        --n;
    }

    auto p4 = (uint32_t*)p1;
    size_t n4 = n >> 2;
    for (size_t i = 0; i < n4; ++i) {
        p4[i] = ~p4[i];
    }

    for (size_t i = n & ~3; i < n; ++i) {
        p1[i] = ~p1[i];
    }
}

void dim(void* p, size_t n)
{
    auto p1 = (uint8_t*)p;

    for (size_t i = 0; i < n; ++i) {
        p1[i] >>= 1;
    }
}

void fade(void* p, size_t n)
{
    auto p1 = (uint8_t*)p;

    for (size_t i = 0; i < n; ++i) {
        if (p1[i] & 0x80)
            p1[i] >>= 1;
        else
            p1[i] <<= 1;
    }
}

void memAnd(void* dst, const void* src, size_t n)
{
    auto d = (uint8_t*)dst;
    auto s = (const uint8_t*)src;

    while (n--) {
        *d++ &= *s++;
    }
}

void memOr(void* dst, const void* src, size_t n)
{
    auto d = (uint8_t*)dst;
    auto s = (const uint8_t*)src;

    while (n--) {
        *d++ |= *s++;
    }
}

void Rect::unionRect(const Rect& r)
{
    if (!valid()) {
        *this = r;
        return;
    }
    if (!r.valid()) {
        return;
    }
    int x1 = std::min(x, r.x);
    int y1 = std::min(y, r.y);
    int x2 = std::max(x + w, r.x + r.w);
    int y2 = std::max(y + h, r.y + r.h);
    x = x1;
    y = y1;
    w = x2 - x1;
    h = y2 - y1;
}

void Rect::unionRects(const Rect& r1, const Rect& r2)
{
    if (!r1.valid()) {
        *this = r2;
        return;
    }
    if (!r2.valid()) {
        *this = r1;
        return;
    }
    x = std::min(r1.x, r2.x);
    y = std::min(r1.y, r2.y);
    w = std::max(r1.x + r1.w, r2.x + r2.w) - x;
    h = std::max(r1.y + r1.h, r2.y + r2.h) - y;
}

void FrameBuffer::line(int x0, int y0, int x1, int y1)
{
#if 0
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
#else
    int sx = 1;
    int sy = 1;
    int dx = x1 - x0;
    int dy = y1 - y0;
    if (x0 > x1) {
        dx = -dx;
        sx = -sx;
    }
    if (y0 > y1) {
        dy = -dy;
        sy = -sy;
    }
#endif
    int err = dx - dy;

    while (true) {
        pset(x0, y0);
        if (x0 == x1 && y0 == y1)
            break;
        int e2 = err << 1;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void FrameBuffer::rect(const Rect* r)
{
    hline(r->x, r->y, r->x + r->w - 1);
    hline(r->x, r->y + r->h - 1, r->x + r->w - 1);
    vline(r->x, r->y, r->y + r->h - 1);
    vline(r->x + r->w - 1, r->y, r->y + r->h - 1);
}

void FrameBuffer::roundRect(const Rect* r, unsigned int radius)
{
    hline(r->x + radius, r->y, r->x + r->w - 1 - radius);
    hline(r->x + radius, r->y + r->h - 1, r->x + r->w - 1 - radius);
    vline(r->x, r->y + radius, r->y + r->h - 1 - radius);
    vline(r->x + r->w - 1, r->y + radius, r->y + r->h - 1 - radius);
    if (radius > 1) {
        // TODO
    }
}
