#ifndef OCHER_FREETYPE_H
#define OCHER_FREETYPE_H

#include <ft2build.h>
#include FT_FREETYPE_H

class FrameBuffer;

class FreeType
{
public:
    FreeType(FrameBuffer *fb);

    void setSize(unsigned int points);
    bool renderGlyph(int glyph, int *penX, int *penY);

protected:
    FT_Library m_lib;
    FT_Face m_face;

    FrameBuffer *m_fb;
};

#endif

