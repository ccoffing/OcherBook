/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_FB_WIDGETS_H
#define OCHER_FB_WIDGETS_H

#include "ux/Event.h"
#include "ux/fb/FrameBuffer.h"

#include <ev.h>

#include <string>
#include <vector>

#define WIDGET_HIDDEN 1  ///< draw will not be called
#define WIDGET_OWNED  2
#define WIDGET_DIRTY  4


class Widget;
class Window;

/**
 * Represents the drawing area of a FrameBuffer and interacts with an EventLoop.
 */
class FbScreen {
public:
    FbScreen();
    ~FbScreen() = default;

    void setFrameBuffer(FrameBuffer* fb);
    void setEventLoop(EventLoop* loop);

    /**
     * Adds a child (transfers ownership).
     */
    void addChild(Window* child);

    /**
     * Adds a child (does not transfer ownership).
     */
//	void addChild(Widget& child);

    void removeChild(Window* child);

    void dispatchEvent(const struct OcherEvent*);

    EventLoop* m_loop;

protected:
    /**
     * Updates the screen:  Causes all invalidated widgets to redraw, and blits to the framebuffer.
     * Done periodically automatically
     */
    void update();

    // std::vector<> m_invalidRects;

    static void timeoutCb(EV_P_ ev_timer* w, int revents);
    static void readyToIdle(EV_P_ ev_prepare* p, int revents);
    static void waking(EV_P_ ev_check* c, int revents);

    FrameBuffer* m_fb;
    ev_timer m_timer;
    ev_prepare m_evPrepare;
    ev_check m_evCheck;

    Rect m_rect;
    std::vector<Widget*> m_children;
};


/**
 * Base class for all UI widgets
 */
class Widget {
public:
    Widget();
    Widget(int x, int y, unsigned int w, unsigned int h);
    virtual ~Widget();

    /**
     * Adds a child (transfers ownership).
     */
    void addChild(Widget* child);
    /**
     * Adds a child (does not transfer ownership).
     */
    void addChild(Widget& child);

    void removeChild(Widget* child);

    void hide()
    {
        m_flags |= WIDGET_HIDDEN;
    }
    void show()
    {
        m_flags &= ~WIDGET_HIDDEN;
    }

    void setRect(int x, int y, int w, int h)
    {
        m_rect.x = x;
        m_rect.y = y;
        m_rect.w = w;
        m_rect.h = h;
    }
    void setPos(int x, int y);
    void setSize(int w, int h);
    void resize(int dx, int dy);

    /**
     * Invalidates the widget, so that it will be redrawn.
     */
    void invalidate();

    /**
     * Invalidates a portion of the widget, so that it will be redrawn.
     * @param rect Widget-relative rectangle to invalidate
     */
    virtual void invalidate(Rect* rect);

    /**
     */
    virtual void draw() = 0;
    Rect drawChildren();

    /**
     * @return -1 handled, -2 pass on, >=0 done
     */
    virtual int evtKey(const struct OcherKeyEvent*)
    {
        return -2;
    }
    virtual int evtMouse(const struct OcherMouseEvent*)
    {
        return -2;
    }
    virtual int evtApp(const struct OcherAppEvent*)
    {
        return -2;
    }
    virtual int evtDevice(const struct OcherDeviceEvent*)
    {
        return -2;
    }

    // TODO: abs or rel?
    Rect m_rect;
    unsigned int m_flags;

    virtual void onAttached()
    {
    }
    virtual void onDetached()
    {
    }

protected:
    FrameBuffer* m_fb;
    Widget* m_parent;
    std::vector<Widget*> m_children;
    // TODO focus
};

#define OWF_CLOSE 1

class Window : public Widget {
public:
    Window();
    Window(int x, int y, unsigned int w, unsigned int h);
    ~Window() = default;

    void draw();
    virtual void drawBorder(Rect* rect);
    virtual void drawTitle(Rect* rect);
    virtual void drawBg(Rect* rect);
    virtual void drawContent(Rect* rect);

    void setTitle(const char* title);
    void maximize();

    uint32_t m_bgColor;
    uint32_t m_borderColor;
    uint8_t m_borderWidth;
    uint32_t m_winflags;

    std::string m_title;

protected:
    // border style:  none, simple, raised
    // bubble arrow:  none, top, right, bottom, left
    // close
};

class Button : public Widget {
public:
    Button(int x, int y, unsigned int w = 0, unsigned int h = 0);
    ~Button() = default;

    void setLabel(const char* label);

    void draw();
    virtual void drawBorder(Rect* rect);
    virtual void drawLabel(Rect* rect);

protected:
    int evtKey(const struct OcherKeyEvent*);
    int evtMouse(const struct OcherMouseEvent*);

    int border;
    std::string m_label;
    // icon;
};

#if 0
class Menu : public Widget {
    int border;
    int separator;
    items;
    icons;
    title;
    selected;
};
#endif


/**
 * @todo listen for powerdown event, stop spinning
 */
class Spinner : public Widget {
public:
    Spinner(EventLoop* loop);
    Spinner(int x, int y, unsigned int w, unsigned int h);
    ~Spinner();
    void start();
    void stop();
    void draw();

protected:
    unsigned int m_state;
    unsigned int m_steps;
    unsigned int m_delayMs;

    static void timeoutCb(EV_P_ ev_timer* w, int revents);
    EventLoop* m_loop;
    ev_timer m_timer;
};

#if 0
class TextEntry : public Window {
};
#endif

#if 0
class OSKeyboard : public Widget {
};
#endif

#if 0
class Label : public Widget {
    label;
    font;
    size;
    attrs;
};
#endif

class Bitmap {
public:
    Bitmap(int _w, int _h, const unsigned char* _bmp) :
        w(_w),
        h(_h),
        bmp(_bmp)
    {
    }
    int w;
    int h;
    const unsigned char* bmp;
};

class Icon : public Widget {
public:
    Icon(int x, int y, Bitmap* _bmp) :
        Widget(x, y, _bmp->w, _bmp->h),
        bmp(_bmp)
    {
    }
    virtual ~Icon()
    {
    }

    void draw();

    Bitmap* bmp;
};

#if 0
class Srubber : public Widget {
    plus / minus vs arrows
    ff arrows
};
#endif

#if 0
class DropDown : public Window {
};
#endif

#if 0
class ButtonBar;
#endif

#endif
