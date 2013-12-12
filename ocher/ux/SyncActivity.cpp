/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#include <sys/types.h>
#include <sys/stat.h>

#include "clc/storage/DirIter.h"
#include "clc/storage/File.h"
#include "clc/storage/Path.h"
#include "clc/support/Logger.h"

#include "ocher/fmt/Format.h"
#include "ocher/settings/Options.h"
#include "ocher/ux/Controller.h"
#include "ocher/ux/Factory.h"
#include "ocher/ux/SyncActivity.h"

#define LOG_NAME "ocher.ux.Sync"


class SyncActivityWork : public EventWork
{
public:
    SyncActivityWork(Controller* controller, const char** files) :
        EventWork(Screen::m_loop),
        m_files(files),
        m_controller(controller)
    {
        start();
    }

protected:
    const char** m_files;
    Controller* m_controller;
    void work();
    void processFile(const char* file);
    void notify();
};

void SyncActivityWork::work()
{
    clc::Log::info(LOG_NAME "Work", "working");
    for (const char* file = *m_files; file; file = *++m_files) {
        processFile(file);
    }
}

void SyncActivityWork::processFile(const char* file)
{
    struct stat s;
    if (stat(file, &s)) {
        clc::Log::warn(LOG_NAME, "%s: stat: %s", file, strerror(errno));
    } else {
        if (S_ISREG(s.st_mode)) {
            Encoding encoding;
            Fmt format = detectFormat(file, &encoding);
            clc::Log::debug(LOG_NAME, "%s: %s", file, Meta::fmtToStr(format));
            if (format != OCHER_FMT_UNKNOWN) {
                Meta* m = new Meta;
                m->format = format;
                m->relPath = file;
                loadMeta(m);
                m_controller->ctx.library.add(m);
            }
        } else if (S_ISDIR(s.st_mode)) {
            clc::Buffer name;
            clc::DirIter dir(file);
            while (dir.getNext(name) == 0 && name.length()) {
                clc::Buffer entryName = clc::Path::join(file, name);
                processFile(entryName.c_str());
            }
        }
    }
}

void SyncActivityWork::notify()
{
    clc::Log::info(LOG_NAME "Work", "notify");
    m_controller->ctx.library.notify();
    m_controller->setNextActivity(ACTIVITY_HOME);
}


void SyncActivity::draw()
{
    clc::Log::debug(LOG_NAME, "draw");
    g_fb->setFg(0xff, 0xff, 0xff);
    g_fb->fillRect(&m_rect);
}

SyncActivity::SyncActivity(Controller* c) :
    m_controller(c)
{
    maximize();

    int w = m_rect.w / 8;
    m_spinner.setRect(m_rect.w/2 - w/2, m_rect.h/2 - w-2, w, w);
    addChild(m_spinner);
}

void SyncActivity::onAttached()
{
    clc::Log::info(LOG_NAME, "attached");
    m_work = new SyncActivityWork(m_controller, opt.files);
    m_spinner.start();
    invalidate();
}

void SyncActivity::onDetached()
{
    clc::Log::info(LOG_NAME, "detached");
    // TODO:  Can be detached not due to work finishing, but being
    // forced out, eg, power saver.  Pause work, don't delete.
    delete m_work;
    m_spinner.stop();
}

