/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>

#include "airbag_fd/airbag_fd.h"

#include "clc/support/Logger.h"
#include "clc/support/LogAppenders.h"

#include "ocher/device/Device.h"
#include "ocher/settings/Options.h"
#include "ocher/settings/Settings.h"
#include "ocher/ux/Controller.h"
#include "ocher/ux/Factory.h"

struct Options opt;

UiFactory* uiFactory;
extern void runBootMenu();


void initCrash()
{
#ifdef OCHER_AIRBAG_FD
    airbag_init_fd(2, 0);
#endif
}

void initLog()
{
    static clc::LogAppenderCFile appender(stderr);
    clc::Logger* l = clc::Log::get("");
    l->setAppender(&appender);
    if (opt.verbose < 0)
        l->setLevel(clc::Log::Fatal);
    else if (opt.verbose == 0)
        l->setLevel(clc::Log::Error);
    else if (opt.verbose == 1)
        l->setLevel(clc::Log::Warn);
    else if (opt.verbose == 2)
        l->setLevel(clc::Log::Info);
    else if (opt.verbose == 3)
        l->setLevel(clc::Log::Debug);
    else
        l->setLevel(clc::Log::Trace);
}

void initSettings()
{
    g_settings.load();
}

void initDebug()
{
    // Before proceeding with startup and initializing the framebuffer, check for a killswitch.
    // Useful when needing to bail to native stack (such as OcherBook vs native stack init-ing
    // framebuffer in incompatible ways).
    if (g_device->fs.m_libraries) {
        for (int i = 0; ; ++i) {
            const char* lib = g_device->fs.m_libraries[i];
            if (! lib)
                break;
            clc::Buffer killswitch(1, "%s/.ocher/kill", lib);
            if (access(killswitch.c_str(), F_OK) == 0) {
                fprintf(stderr, "OcherBook exiting because of '%s' killswitch\n", killswitch.c_str());
                exit(1);
            }
        }
    }
}

void usage(const char* msg)
{
    printf(
        "OcherBook  Copyright (C) 2012 Chuck Coffing  <clc@alum.mit.edu>\n"
        "License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n"
        "\n");
    if (msg) {
        printf("%s\n\n", msg);
    }
    printf(
      // 12345678901234567890123456789012345678901234567890123456789012345678901234567890
        "Usage:  ocher [OPTIONS]... [FILE]...\n"
        "\n"
        "-b,--boot            Present boot menu; nonzero exit means run other firmware.\n"
        "-f,--flatten         Flatten (do not show to user) the directory heirarchy.\n"
        "-t,--test            Test (validate) the epubs rather than view.\n"
        "-h,--help            Help.\n"
        "-v,--verbose         Increase logging verbosity.\n"
        "-q,--quiet           Quiet; decrease logging verbosity.\n"
        "   --list-drivers    List all available output drivers.  Each driver consists of\n"
        "                     a font renderer driving a hardware device.\n"
        "   --driver <driver> Use a specific driver.\n"
        "\n"
        "Multiple files and/or directories may be specified, and will override any\n"
        "platform specific search paths.  Directories will be searched recursively.\n"
        "\n"
    );
    exit(msg ? 0 : 1);
}

#define OPT_DRIVER 256
#define OPT_LIST_DRIVERS 257

int main(int argc, char** argv)
{
    bool listDrivers = false;
    bool bootMenu = false;
    const char* driverName = 0;

    struct option long_options[] =
    {
        {"boot",         no_argument,       0,'b'},
        {"flatten",      no_argument,       0,'f'},
        {"help",         no_argument,       0,'h'},
        {"quiet",        no_argument,       0,'q'},
        {"test",         no_argument,       0,'t'},
        {"verbose",      no_argument,       0,'v'},
        {"driver",       required_argument, 0, OPT_DRIVER},
        {"list-drivers", no_argument,       0, OPT_LIST_DRIVERS},
        {0, 0, 0, 0}
    };

    while (1) {
        // getopt_long stores the option index here.
        int option_index = 0;

        int c = getopt_long(argc, argv, "d:bfhtvq", long_options, &option_index);
        if (c == -1)
            break;
        switch (c) {
            case 0:
                break;
            case 'b':
                bootMenu = true;
                break;
            case 'v':
                opt.verbose++;
                break;
            case 'q':
                opt.verbose--;
                break;
            case 'h':
                usage(0);
                break;
            case OPT_DRIVER:
                driverName = optarg;
                break;
            case OPT_LIST_DRIVERS:
                listDrivers = true;
                break;
            default:
                usage("Unknown argument");
                break;
        }
    }

    initCrash();
    initLog();
    initDevice();
    initSettings();
    initDebug();

    for (unsigned int i = 0; i < getDrivers().size(); ++i) {
        UiFactory* factory = (UiFactory*)getDrivers().get(i);

        if (listDrivers) {
            printf("\t%s\n", factory->getName());
        } else if (driverName) {
            if (strcmp(factory->getName(), driverName) == 0) {
                clc::Log::debug("ocher", "Attempting to init the '%s' driver", driverName);
                if (!factory->init()) {
                    clc::Log::warn("ocher", "Failed to init the '%s' driver", driverName);
                    return 1;
                }
                uiFactory = factory;
                break;
            }
        } else {
            if (factory->init()) {
                uiFactory = factory;
                break;
            }
        }
    }
    if (listDrivers) {
        return 0;
    }
    if (! uiFactory) {
        printf("No suitable output driver found\n");
        return 1;
    }
    clc::Log::info("ocher", "Using the '%s' driver", uiFactory->getName());

    if (optind < argc) {
        opt.files = (const char**)&argv[optind];
    } else {
        opt.files = g_device->fs.m_libraries;
    }
    if (!opt.files || !opt.files[0]) {
        uiFactory->deinit();
        usage("Please specify one or more files or directories.");
    }

#ifdef OCHER_TARGET_KOBO
    // Kobo rc scripts start animate.sh, which shows an animation while nickel is starting.
    // Kill that here (so it doesn't overlay the boot menu) to simplify installation steps.
    // TODO: Remove when more closely integrated.
    system("killall on-animator.sh");
    sleep(1);
#endif

    uiFactory->populate();
    g_device->fs.initWatches();

    Controller c;
    c.run(bootMenu ? ACTIVITY_BOOT : ACTIVITY_SYNC);

    uiFactory->deinit();

    return 0;
}

