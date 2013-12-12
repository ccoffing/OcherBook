/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#ifndef OCHER_UX_FB_RLEIMAGE_H
#define OCHER_UX_FB_RLEIMAGE_H

#include <stdint.h>


/**
 * Used to store simple images, such as line drawings.
 * Two consecutive identical bytes indicate a run; the next byte indicates how
 * many additional such bytes follow (0-255).
 */
class RleBitmap
{
public:
    RleBitmap();

    /**
     * Imports the uncompressed buffer.
     */
    static void pack(uint8_t* u, unsigned int len, uint8_t* p);

    /**
     * Unpacks the next chunk (scanline?)
     */
    int unpack(uint8_t* d, unsigned int len);

    inline void resetUnpack() { m_off = 0; }

protected:
    uint8_t* m_p;
    unsigned int m_len;  ///< Length of packed data
    unsigned int m_off;  ///< Offset of next byte to unpack
};

#endif
