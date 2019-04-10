/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_META_H
#define OCHER_META_H

#include "ux/Pagination.h"

#include <string>
#include <sys/time.h>

enum Encoding {
    OCHER_ENC_UNKNOWN,
    OCHER_ENC_UTF8,
    OCHER_ENC_UTF16BE,
    OCHER_ENC_UTF16LE,
    OCHER_ENC_UTF32BE,
    OCHER_ENC_UTF32LE,
};

enum Fmt {
    OCHER_FMT_UNKNOWN,
    OCHER_FMT_EPUB,
    OCHER_FMT_TEXT,
    OCHER_FMT_HTML,
};

/** User-specific and book-specific metadata.
 */
class BookRecord {
public:
    BookRecord() :
        activePage(0),
        touched(0),
        shortlist(0)
    {
    }

    int activePage;  // TODO:  can't use page number; varies by device/settings

    void touch();
    time_t touched;

    int shortlist;

    // TODO: tags

    // TODO: dates
    // TODO: highlights
    // TODO: annotations
    // TODO: notes
};

/** Metadata associated with each book.
 *
 * This is the root object for each e-book such as the format, the layout, metadata, user's reading
 * record, etc.  Note the separation between book-specific and user-specific data.
 *
 * TODO: refcounted by views (Shelf)?
 */
class Meta {
public:
    Meta();

    static const char* fmtToStr(Fmt fmt);

    Fmt format;
    Encoding encoding;

    std::string relPath;  // TODO: for now, full path

    std::string author;
    std::string title;
    std::string language;

    std::string icon;

    Pagination m_pagination;
    int pages; // is this even meaningful?

    BookRecord record;

    unsigned int percentRead();
};

void loadMeta(Meta* meta);

#endif
