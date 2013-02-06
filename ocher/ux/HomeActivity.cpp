#include "clc/support/Logger.h"

#include "ocher/settings/Settings.h"
#include "ocher/shelf/Meta.h"
#include "ocher/ux/Controller.h"
#include "ocher/ux/Factory.h"
#include "ocher/ux/HomeActivity.h"
#include "ocher/ux/fb/FontEngine.h"
#include "ocher/ux/fb/Widgets.h"

#define LOG_NAME "ocher.ux.Home"


class HomeCanvas : public Canvas
{
public:
    HomeCanvas(Context& ctx);
    ~HomeCanvas();

    int evtKey(struct OcherEvent*);
    int evtMouse(struct OcherEvent*);

    Rect draw(Pos* pos);

protected:
    Context& m_ctx;
    float coverRatio;
#define NUM_CLUSTER_BOOKS 5
    Rect books[NUM_CLUSTER_BOOKS];
    Rect shortlist[5];
    Rect m_browseLabel;
};

HomeCanvas::HomeCanvas(Context& ctx) :
    m_ctx(ctx),
    coverRatio(1.6)
{
    int dx = settings.smallSpace;
    int dy = settings.smallSpace;

    books[0].x = m_rect.w/15;
    books[0].y = m_rect.h/5;
    books[0].w = m_rect.w/2.8;
    books[0].h = books[0].w*coverRatio;

    books[1].x = books[0].x + books[0].w + dx;
    books[1].y = m_rect.h/6;
    books[1].w = m_rect.w/4;
    books[1].h = books[1].w*coverRatio;

    books[2].x = books[1].x + books[1].w + dx;
    books[2].w = m_rect.w/5;
    books[2].h = books[2].w*coverRatio;
    books[2].y = books[1].y + books[1].h - books[2].h;

    books[3].x = books[1].x;
    books[3].y = books[1].y + books[1].h + dy;
    books[3].w = books[2].w;
    books[3].h = books[2].h;

    books[4] = books[3];
    books[4].x += books[3].w + dx;
    books[4].w -= 2*dx;
    books[4].h = books[4].w*coverRatio;
}

HomeCanvas::~HomeCanvas()
{
}

int HomeCanvas::evtKey(struct OcherEvent*)
{
    return -1;
}

int HomeCanvas::evtMouse(struct OcherEvent* evt)
{
    if (evt->subtype == OEVT_MOUSE1_UP) {
        Pos* pos = (Pos*)&evt->mouse;
        const clc::List& metas = m_ctx.library.getList();
        for (unsigned int i = 0; i < NUM_CLUSTER_BOOKS; i++) {
            Meta* meta = (Meta*)metas.itemAt(i);
            if (!meta) {
                clc::Log::trace(LOG_NAME, "book %d has no meta", i);
                continue;
            }
            if (books[i].contains(pos)) {
                clc::Log::info(LOG_NAME, "book %d selected %p", i, meta);
                Rect r = books[i];
                r.inset(-2);
                g_fb->roundRect(&r, 3);
                r.inset(-1);
                g_fb->roundRect(&r, 4);
                g_fb->update(&r);
                g_fb->sync();
                m_ctx.selected = meta;
                return ACTIVITY_READ;
            }
        }
        if (m_browseLabel.contains(pos)) {
            return ACTIVITY_LIBRARY;
        }
        // TODO: look at shortlist
    }
    return -1;
}

Rect HomeCanvas::draw(Pos*)
{
    Rect drawn;
    drawn.setInvalid();

    if (m_flags & WIDGET_DIRTY) {
        clc::Log::debug(LOG_NAME, "draw");
        m_flags &= ~WIDGET_DIRTY;
        drawn = m_rect;

        g_fb->setFg(0xff, 0xff, 0xff);
        g_fb->fillRect(&m_rect);
        g_fb->setFg(0, 0, 0);

        FontEngine fe;
        fe.setSize(12);
        fe.apply();
        Rect r;
        Pos pos;
        const clc::List& metas = m_ctx.library.getList();
        for (unsigned int i = 0; i < NUM_CLUSTER_BOOKS; ++i) {
            r = books[i];
            r.inset(-1);
            g_fb->rect(&r);
            r.inset(-1);
            g_fb->roundRect(&r, 1);
            r.inset(2);

            Meta* meta = (Meta*)metas.itemAt(i);
            uint8_t c = meta ? 0xf0 : 0xd0;
            g_fb->setFg(c, c, c);
            g_fb->fillRect(&r);
            g_fb->setFg(0, 0, 0);
            if (meta) {
                pos.x = 0;
                pos.y = fe.m_cur.ascender;
                r.inset(2);
                fe.renderString(meta->title.c_str(), meta->title.length(), &pos, &r, FE_YCLIP | FE_WRAP);
            }
        }

        fe.setSize(18);
        fe.apply();
        pos.x = 0; pos.y = 100;
        fe.renderString("HOME", 4, &pos, &m_rect, FE_XCENTER);

        // Shortlist
        fe.setSize(14);
        fe.apply();
        pos.x = books[0].x;
        pos.y = books[3].y + books[3].h + fe.m_cur.ascender + settings.smallSpace;
        fe.renderString("Shortlist", 9, &pos, &m_rect, 0);

        // TODO simplify, abstract into labels
        {
            fe.setSize(12);
            fe.setItalic(1);
            fe.apply();
            Rect lbox;
            lbox.x = 0;
            lbox.y = pos.y;
            Glyph* glyphs[13];
            const char* text = "Browse all...";
            fe.plotString(text, strlen(text), &glyphs[0], &lbox);
            // TODO  right justify against lbox (remove plotString call); get bbox returned
            pos.x = m_rect.w - books[0].x - lbox.w;
            fe.renderString(text, strlen(text), &pos, &m_rect, 0, &m_browseLabel);
            m_browseLabel.inset(-settings.smallSpace);
        }

        pos.y += fe.m_cur.underlinePos + settings.smallSpace;
        g_fb->hline(books[0].x, pos.y, m_rect.w - books[0].x);
        pos.y++;
        g_fb->hline(books[0].x, pos.y, m_rect.w - books[0].x);

        pos.x = books[0].x;
        pos.y += settings.smallSpace;

        {
            const clc::List& shortList = m_ctx.shortList.getList();
            int margin = books[0].x;

            int h = m_rect.y + m_rect.h - pos.y - margin;
            int w = h / coverRatio;
            Rect sl(pos.x, pos.y, w, h);
            while (sl.x + sl.w <= m_rect.w - margin) {
                g_fb->roundRect(&sl, 1);
                sl.inset(-1);
                g_fb->roundRect(&sl, 2);

                sl.x += sl.w + settings.smallSpace;
            }
        }

    }
    Rect d2 = drawChildren(m_rect.pos());
    drawn.unionRect(&d2);

#if 0
    g_fb->byLine(&g_fb->bbox, dim);
    Rect popup(25, 200, 550, 400);
    g_fb->rect(&popup);
    popup.inset(1);
    g_fb->rect(&popup);
    popup.inset(1);
    g_fb->setFg(0xff, 0xff, 0xff);
    g_fb->fillRect(&popup);
#endif

    return drawn;
}


HomeActivity::HomeActivity(Controller* c) :
    m_controller(c)
{
}

Activity HomeActivity::run()
{
    clc::Log::info(LOG_NAME, "run");
    HomeCanvas c(m_controller->ctx);

    SystemBar& systemBar = m_controller->ui.m_systemBar;
    systemBar.m_sep = false;
    systemBar.m_title.clear();
    systemBar.show();
    c.addChild(systemBar);

    c.refresh(true);
    return (Activity)g_loop->run(&c);
}

