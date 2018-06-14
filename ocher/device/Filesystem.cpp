/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "device/Filesystem.h"

#include "ux/Event.h"

#include "settings/Options.h"
#include "util/File.h"
#include "util/Logger.h"
#include "util/Path.h"

#include <cstdlib>
#include <sys/stat.h>
#if !defined(__HAIKU__)
#include <pwd.h>
#include <unistd.h>
#endif
#ifdef __linux__
#include <sys/inotify.h>
#endif

#define LOG_NAME "ocher.dev.fs"

#ifndef OCHER_TARGET_KOBO
static std::string settingsDir()
{
    std::string dir;

#if defined(__HAIKU__)
    dir = "/boot/home/config/settings";
#else
    const char* e = nullptr;
    if (!e)
        e = getenv("HOME");
    if (!e) {
        struct passwd* p = getpwuid(getuid());
        if (p) {
            e = p->pw_dir;
        }
    }
    if (!e) {
        e = "/tmp";
    }
    dir = e;
#if defined(__APPLE__)
    dir = Path::join(dir, "Library");
    dir = Path::join(dir, "Application Support");
#endif
#endif
    return dir;
}
#endif

Filesystem::Filesystem()
{
#ifdef OCHER_TARGET_KOBO
    m_libraries = new const char*[3];
    m_libraries[0] = "/mnt/sd";
    m_libraries[1] = "/mnt/onboard";
    m_libraries[2] = 0;
    m_home = "/mnt/onboard/.ocher";
    m_settings = "/mnt/onboard/.ocher/settings";
#else
    std::string s = settingsDir();
#if defined(__HAIKU__) || defined(__APPLE__)
    s = Path::join(s, "OcherBook");
#else
    s = Path::join(s, ".OcherBook");
#endif
    m_home = s;
    ::mkdir(s.c_str(), 0775);
    s = Path::join(s, "settings");
    m_settings = s;
#endif
}

Filesystem::~Filesystem()
{
    delete[] m_libraries;
}

void Filesystem::initWatches(Options* options, EventLoop* loop)
{
#ifdef __linux__
    if (m_notifyFd == -1) {
        m_notifyFd = inotify_init();
        if (m_notifyFd == -1) {
            Log::error(LOG_NAME, "inotify_init failed: %s", strerror(errno));
            return;
        }

        ev_io_init(&m_watcher, _watchCb, m_notifyFd, EV_READ);
        m_watcher.data = this;
        ev_io_start(loop->evLoop, &m_watcher);
    }

    // TODO  Watch Settings->libraries instead
    const char** files = options->files;

    if (!files) {
	Log::info(LOG_NAME, "No directories to watch");
        return;
    }
    for (int i = 0; files[i]; ++i) {
        int wd = inotify_add_watch(m_notifyFd, files[i], IN_CREATE | IN_DELETE | IN_MOVED_FROM | IN_MOVED_TO);
        if (wd == -1) {
            Log::error(LOG_NAME, "inotify_add_watch(\"%s\") failed: %s", files[i], strerror(errno));
            continue;
        }
        Log::info(LOG_NAME, "Watching '%s'", files[i]);
    }
#endif
}

void Filesystem::deinitWatches()
{
#ifdef __linux__
    if (m_notifyFd != -1)
        close(m_notifyFd);
#endif
}

#ifdef __linux__
void Filesystem::_watchCb(struct ev_loop* loop, ev_io* watcher, int revents)
{
    static_cast<Filesystem*>(watcher->data)->watchCb();
}

void Filesystem::watchCb()
{
    char buff[4096];
    ssize_t len = read(m_notifyFd, buff, sizeof(buff));
    ssize_t i = 0;
    while (i < len) {
        struct inotify_event* pevent = reinterpret_cast<struct inotify_event*>(&buff[i]);

        const char* filename = pevent->len == 0 ? "" : pevent->name;

        if (pevent->mask & IN_ACCESS) {
            Log::debug(LOG_NAME, "inotify ACCESS \"%s\"", filename);
        }
        if (pevent->mask & IN_ATTRIB) {
            Log::debug(LOG_NAME, "inotify ATTRIB \"%s\"", filename);
        }
        if (pevent->mask & IN_CLOSE_WRITE) {
            Log::debug(LOG_NAME, "inotify CLOSE_WRITE \"%s\"", filename);
        }
        if (pevent->mask & IN_CLOSE_NOWRITE) {
            Log::debug(LOG_NAME, "inotify CLOSE_NOWRITE \"%s\"", filename);
        }
        if (pevent->mask & IN_CREATE) {
            Log::debug(LOG_NAME, "inotify CREATE \"%s\"", filename);
        }
        if (pevent->mask & IN_DELETE) {
            Log::debug(LOG_NAME, "inotify DELETE \"%s\"", filename);
        }
        if (pevent->mask & IN_DELETE_SELF) {
            Log::debug(LOG_NAME, "inotify DELETE_SELF \"%s\"", filename);
        }
        if (pevent->mask & IN_MODIFY) {
            Log::debug(LOG_NAME, "inotify MODIFY \"%s\"", filename);
        }
        if (pevent->mask & IN_MOVE_SELF) {
            Log::debug(LOG_NAME, "inotify MOVE_SELF \"%s\"", filename);
        }
        if (pevent->mask & IN_MOVED_FROM) {
            Log::debug(LOG_NAME, "inotify MOVED_FROM \"%s\"", filename);
        }
        if (pevent->mask & IN_MOVED_TO) {
            Log::debug(LOG_NAME, "inotify MOVED_TO \"%s\"", filename);
        }
        if (pevent->mask & IN_OPEN) {
            Log::debug(LOG_NAME, "inotify OPEN \"%s\"", filename);
        }

        // TODO  Associate wd with the library and look up
        dirChanged("", filename);

        i += sizeof(struct inotify_event) + pevent->len;
    }
}
#endif
