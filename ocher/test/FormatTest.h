/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#include "ocher/fmt/Format.h"

SUITE(Format)
{
    TEST(Detect)
    {
        PRINT_PROGRESS;
        REQUIRE_RESOURCES_DIR;

        clc::Buffer fn;
        Encoding enc;
        Fmt fmt;
        
        fn = resourcesDir;
        fn += "/";
        fn += "simple.txt";
        fmt = detectFormat(fn.c_str(), &enc);
        CHECK_EQUAL(OCHER_ENC_UTF8, enc);
        CHECK_EQUAL(OCHER_FMT_TEXT, fmt);

        fn = resourcesDir;
        fn += "/";
        fn += "simple.bin";
        fmt = detectFormat(fn.c_str(), &enc);
        CHECK_EQUAL(OCHER_ENC_UNKNOWN, enc);
        CHECK_EQUAL(OCHER_FMT_UNKNOWN, fmt);
        fn = resourcesDir;

        fn += "/";
        fn += "simple.html";
        fmt = detectFormat(fn.c_str(), &enc);
        CHECK_EQUAL(OCHER_ENC_UTF8, enc);
        CHECK_EQUAL(OCHER_FMT_HTML, fmt);

        // epub
    }
}

