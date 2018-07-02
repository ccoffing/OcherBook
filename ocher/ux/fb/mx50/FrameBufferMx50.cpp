/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ux/fb/mx50/FrameBufferMx50.h"

#include "util/Logger.h"

#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define uint unsigned int
#include <linux/mxcfb.h>

#define LOG_NAME "ocher.mx50"

FrameBufferMx50::~FrameBufferMx50()
{
    if (m_fd != -1) {
        close(m_fd);
        m_fd = -1;
    }
}

bool FrameBufferMx50::init()
{
    const char* dev = "/dev/fb0";

    m_fd = open(dev, O_RDWR);
    if (m_fd == -1) {
        Log::error(LOG_NAME, "Failed to open %s: %s", dev, strerror(errno));
        goto fail;
    }

    // Get fixed screen information
    if (ioctl(m_fd, FBIOGET_FSCREENINFO, &finfo) == -1) {
        Log::error(LOG_NAME, "Failed to get fixed screen info: %s", strerror(errno));
        goto fail1;
    }
    char id[sizeof(finfo.id) + 1];
    memcpy(id, finfo.id, sizeof(finfo.id));
    id[sizeof(finfo.id)] = 0;
    Log::info(LOG_NAME, "id: %s", id);

    // Get variable screen information
    if (ioctl(m_fd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        Log::error(LOG_NAME, "Failed to get variable screen info: %s", strerror(errno));
        goto fail1;
    }

    // Configure for what we actually want
    vinfo.bits_per_pixel = 8;
    vinfo.grayscale = 1;
    // 0 is landscape right handed, 1 portait upside-down, 2 , 3 portrait
    vinfo.rotate = 3;
    if (ioctl(m_fd, FBIOPUT_VSCREENINFO, &vinfo) == -1) {
        Log::error(LOG_NAME, "Failed to set variable screen info: %s", strerror(errno));
        goto fail1;
    }

    Log::info(LOG_NAME, "virtual %dx%d", vinfo.xres_virtual, vinfo.yres_virtual);
    // Figure out the size of the screen in bytes
    m_fbSize = vinfo.xres_virtual * vinfo.yres_virtual * vinfo.bits_per_pixel / 8;

    // Map the device to memory
    m_fb = (char*)mmap(nullptr, m_fbSize, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, 0);
    if (m_fb == MAP_FAILED) {
        Log::error(LOG_NAME, "mmap: %s", strerror(errno));
        goto fail1;
    }

    setBg(0xff, 0xff, 0xff);
    clear();
    update(nullptr, true);

    bbox.x = bbox.y = 0;
    bbox.w = width();
    bbox.h = height();
    Log::info(LOG_NAME, "fb %d %d %d %d", bbox.x, bbox.y, bbox.w, bbox.h);

    return true;

fail1:
    close(m_fd);
    m_fd = -1;
fail:
    return false;
}

unsigned int FrameBufferMx50::height()
{
    return 800;
}

unsigned int FrameBufferMx50::width()
{
    return 600;
}

unsigned int FrameBufferMx50::dpi()
{
    // Kobo Touch: 600x800; ~7" diagonal
    return 140;
}

static inline uint8_t getColor(uint8_t r, uint8_t b, uint8_t g)
{
    return (((uint16_t)r * 61) + ((uint16_t)g * 174) + ((uint16_t)b * 21)) >> 8;
}

void FrameBufferMx50::setFg(uint8_t r, uint8_t b, uint8_t g)
{
    m_fgColor = getColor(r, b, g);
}

void FrameBufferMx50::setBg(uint8_t r, uint8_t b, uint8_t g)
{
    m_bgColor = getColor(r, b, g);
}

void FrameBufferMx50::clear()
{
    memset(m_fb, m_bgColor, m_fbSize);
}

void FrameBufferMx50::pset(int x, int y)
{
    *(((unsigned char*)m_fb) + y * vinfo.xres_virtual + x) = m_fgColor;
}

inline void FrameBufferMx50::hline(int x1, int y, int x2)
{
    int dx = x2 - x1;

    if (dx < 0)
        dx = -dx;
    memset(m_fb + y * vinfo.xres_virtual + x1, m_fgColor, dx + 1);
}

void FrameBufferMx50::fillRect(const Rect* r)
{
    int y2 = r->y + r->h;

    for (int y = r->y; y < y2; ++y) {
        hline(r->x, y, r->x + r->w - 1);
    }
}

void FrameBufferMx50::byLine(const Rect* r, void (*fn)(void* p, size_t n))
{
    int y2 = r->y + r->h;

    for (int y = r->y; y < y2; ++y) {
        fn(((unsigned char*)m_fb) + y * vinfo.xres_virtual + r->x, r->w);
    }
}

void FrameBufferMx50::blit(const unsigned char* p, int x, int y, int w, int h, const Rect* userClip)
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

    Log::trace(LOG_NAME, "blit clipped %d %d %d %d", x, y, w, h);
    unsigned char* dst = ((unsigned char*)m_fb) + y * vinfo.xres_virtual + x;
    for (int i = 0; i < h; ++i) {
        memAnd(dst, p, w);
        dst += vinfo.xres_virtual;
        p += rectWidth;
    }
}

int FrameBufferMx50::update(const Rect* r, bool full)
{
    Rect _r;

    if (!r) {
        _r.x = _r.y = 0;
        _r.w = width();
        _r.h = height();
        r = &_r;
    }
    if (m_needFull) {
        m_needFull = false;
        full = true;
    }
    Log::info(LOG_NAME, "update %d %d %u %u", r->x, r->y, r->w, r->h);
    struct mxcfb_update_data region;

    region.update_region.left = r->x;
    region.update_region.top = r->y;
    region.update_region.width = r->w;
    region.update_region.height = r->h;
    region.waveform_mode = WAVEFORM_MODE_AUTO;
    region.update_mode = full ? UPDATE_MODE_FULL : UPDATE_MODE_PARTIAL;
    region.update_marker = ++m_marker;
    region.temp = TEMP_USE_AMBIENT;
    region.flags = 0;

    if (ioctl(m_fd, MXCFB_SEND_UPDATE, &region) == -1) {
        Log::error(LOG_NAME, "MXCFB_SEND_UPDATE(%d, %d, %d, %d, %d): %s",
                r->x, r->y, r->w, r->h, m_marker, strerror(errno));
    } else {
        Log::info(LOG_NAME, "update %d started", m_marker);
    }

    return m_marker;
}

void FrameBufferMx50::sync()
{
    waitUpdate(m_marker);
}

void FrameBufferMx50::waitUpdate(int marker)
{
    if (ioctl(m_fd, MXCFB_WAIT_FOR_UPDATE_COMPLETE, &marker) == -1) {
        Log::error(LOG_NAME, "MXCFB_WAIT_FOR_UPDATE_COMPLETE(%d): %s", marker, strerror(errno));
    } else {
        Log::info(LOG_NAME, "update %d done", marker);
    }
}

void FrameBufferMx50::setPixelFormat()
{
    vinfo.bits_per_pixel = 8;
    vinfo.grayscale = GRAYSCALE_8BIT;
    int retval = ioctl(m_fd, FBIOPUT_VSCREENINFO, &vinfo);
    if (retval)
        Log::error(LOG_NAME, "FBIOPUT_VSCREENINFO: %s", strerror(errno));
}

void FrameBufferMx50::setAutoUpdateMode(bool autoUpdate)
{
    uint32_t mode;

    mode = autoUpdate ? AUTO_UPDATE_MODE_AUTOMATIC_MODE : AUTO_UPDATE_MODE_REGION_MODE;
    int retval = ioctl(m_fd, MXCFB_SET_AUTO_UPDATE_MODE, &mode);
    if (retval)
        Log::error(LOG_NAME, "MXCFB_SET_AUTO_UPDATE_MODE(%d): %s", mode, strerror(errno));
}

void FrameBufferMx50::setUpdateScheme()
{
#if 0
    vinfo.scheme = UPDATE_SCHEME_SNAPSHOT;
    int retval = ioctl(m_fd, MXCFB_SET_UPDATE_SCHEME, &vinfo);
    if (retval)
        Log::error(LOG_NAME, "MXCFB_SET__UPDATE_SCHEME: %s", strerror(errno));
#endif
}
