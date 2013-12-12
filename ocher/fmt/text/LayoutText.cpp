/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#include "ocher/fmt/text/Text.h"
#include "ocher/fmt/text/LayoutText.h"


LayoutText::LayoutText(Text* text) : m_text(text)
{
    int sawNl = 0;
    unsigned int n = m_text->m_text.size();
    const char* raw = m_text->m_text.data();
    for (unsigned int i = 0; i < n; ++i) {
        // \n\n means real line break; otherwise reflow text
        if (raw[i] == '\n') {
            if (sawNl) {
                outputNl();
                outputBr();
                continue;
            } else
                sawNl = 1;
        } else if (raw[i] == '\f') {
            outputPageBreak();
            continue;
        } else {
            sawNl = 0;
        }
        outputChar(raw[i]);
    }
    flushText();
}
