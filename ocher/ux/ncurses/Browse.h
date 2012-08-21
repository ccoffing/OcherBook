#ifndef OCHER_BROWSE_CURSES_H
#define OCHER_BROWSE_CURSES_H

#include <vector>

#include "clc/data/Buffer.h"
#include "clc/tui/Tui.h"

#include "ocher/ux/Browse.h"


class BrowseCurses : public Browse, clc::Window
{
public:
    BrowseCurses(clc::Tui &tui);

    void repaint();
};


#endif


