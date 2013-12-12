/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#ifndef OCHER_OPTIONS_H
#define OCHER_OPTIONS_H

struct Options {
    Options() :
        verbose(0),
        files(0),
        inFd(0),
        outFd(1) {}

    int verbose;

    const char** files;

    int inFd;
    int outFd;
};

extern struct Options opt;

#endif

