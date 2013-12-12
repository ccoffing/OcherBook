/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#ifndef OCHER_CSS_H
#define OCHER_CSS_H

#include "clc/data/Buffer.h"

// http://www.w3schools.com/cssref/css_selectors.asp
//
// http://www.charlespetzold.com/blog/2011/12/Parsing-CSS-for-EPUB.html
//
// http://idpf.org/epub/20/spec/OPS_2.0.1_draft.htm#Section3.3

// sample APIs
// http://packages.python.org/cssutils/

// @page
// @font-face
// @media


class CssRule
{
public:
};

class CssParser
{
public:
    CssRule* parseString(clc::Buffer &b);
    //CssRule* parseFile(clc::Buffer &filename);
    //parseUrl
    //parseStyle
protected:
    clc::Buffer parseElement();
};

//CSSPageRule(selectorText=None, style=None, parentRule=None, parentStyleSheet=None, readonly=False)

class CssStyleSheet
{
public:
    // encoding
    // rules
    //
};


#endif

