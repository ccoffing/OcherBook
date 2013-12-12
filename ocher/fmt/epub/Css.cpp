/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#include "ocher/fmt/epub/Css.h"


CssParser::CssParser()
{
}

//isspace // CSS does not define \v as space

clc::Buffer CssParser::parseElement()
{
    
}

CssParser::parseElements()
{
}

CssRule* CssParser::parseString(clc::Buffer &b)
{
    const char* p = b.c_str();

    clc::Buffer tmp;

    while (*p) {
        char c = *p;
        if (c == '.') {
            // CSSv1  .foo selects all elements with class="foo"
            tmp = parseElement();
        } else if (c == '#') {
            // CSSv1  #foo selects all elements with id="foo"
            tmp = parseElement();
        } else if (c == ':') {
            tmp = parseElement();
            if (tmp == "link") {
                // CSSv1  link
            } else if (tmp == "visited") {
                // CSSv1  visited
            } else if (tmp == "active") {
                // CSSv1  active
            } else if (tmp == "hover") {
                // CSSv1  hover
            } else if (tmp == "focus") {
                // CSSv2  focus
            } else if (tmp == "first-letter") {
                // CSSv1  first-letter
            } else if (tmp == "first-line") {
                // CSSv1  first-line
            } else {
            }
        } else {
            parseElements();
        }

    }

}
