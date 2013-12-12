/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#include "clc/storage/File.h"
#include "clc/support/Logger.h"
#include "ocher/fmt/text/Text.h"

#define LOG_NAME "ocher.text"


Text::Text(const char* filename)
{
    clc::File f(filename);
    f.readRest(m_text);
    clc::Log::debug(LOG_NAME, "Loaded %lu bytes", (unsigned long)m_text.size());
}
