/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "Container.h"
#include "settings/Options.h"
#include "ux/Controller.h"

#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <getopt.h>
#include <unistd.h>


void usage(const char* msg = nullptr)
{
    printf(
            "OcherBook  Copyright (C) 2015 Chuck Coffing  <clc@alum.mit.edu>\n"
            "License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n"
            "\n");
    if (msg != nullptr) {
        printf("%s\n\n", msg);
    }
    printf(
          // 12345678901234567890123456789012345678901234567890123456789012345678901234567890
            "Usage:  ocher [OPTIONS]... [FILE]...\n"
            "\n"
            "-b,--boot              Present boot menu; nonzero exit means run other firmware.\n"
            "-f,--flatten           Flatten (do not show to user) the directory heirarchy.\n"
            "-h,--help              Help.\n"
            "-v,--verbose           Increase logging verbosity.\n"
            "-q,--quiet             Quiet; decrease logging verbosity.\n"
            "   --key <key>=<value> Set the global <key> to <value>\n"
            "   --list-drivers      List all available output drivers.  Each driver consists of\n"
            "                       a font renderer driving a hardware device.\n"
            "   --driver <driver>   Use a specific driver.\n"
#ifndef DOCTEST_CONFIG_DISABLE
            "   --test ...          Runs unit tests, before fully launching.  Remaining\n"
            "                       arguments are passed to doctest.\n"
#endif
            "\n"
            "Multiple files and/or directories may be specified, and will override any\n"
            "platform specific search paths.  Directories will be searched recursively.\n"
            "\n"
            );
    exit(msg != nullptr ? 0 : 1);
}

#define OPT_KEY 256
#define OPT_DRIVER 257
#define OPT_LIST_DRIVERS 258
#define OPT_TEST 259

int main(int argc, char** argv)
{
    bool listDrivers = false;
    auto opt = new Options;

    struct option long_options[] =
    {
        { "boot",         no_argument,       nullptr, 'b' },
        { "flatten",      no_argument,       nullptr, 'f' },
        { "help",         no_argument,       nullptr, 'h' },
        { "quiet",        no_argument,       nullptr, 'q' },
        { "verbose",      no_argument,       nullptr, 'v' },
        { "key",          required_argument, nullptr, OPT_KEY },
        { "driver",       required_argument, nullptr, OPT_DRIVER },
        { "list-drivers", no_argument,       nullptr, OPT_LIST_DRIVERS },
#ifndef DOCTEST_CONFIG_DISABLE
        { "test",         no_argument,       nullptr, OPT_TEST },
#endif
        { nullptr, 0, nullptr, 0 }
    };

    while (true) {
        // getopt_long stores the option index here.
        int option_index = 0;

        int ch = getopt_long(argc, argv, "d:bfhvq", long_options, &option_index);
        if (ch == -1)
            break;
        switch (ch) {
        case 0:
            break;
        case 'b':
            opt->bootMenu = true;
            break;
        case 'v':
            opt->verbose++;
            break;
        case 'q':
            opt->verbose--;
            break;
        case 'h':
            usage();
            break;
        case OPT_KEY:
        {
            char* split = strchr(optarg, '=');
            if (split) {
                *split = 0;
                opt->keys[optarg] = split + 1;
            } else {
                opt->keys[optarg] = "";
            }
            break;
        }
        case OPT_DRIVER:
            opt->driverName = optarg;
            break;
        case OPT_LIST_DRIVERS:
            listDrivers = true;
            break;
        case OPT_TEST:
        {
            //context.applyCommandLine(argc - optind + 1, argv + optind - 1);
            opt->test = true;
            break;
        }
        default:
            usage("Unknown argument");
            break;
        }
    }

    if (optind < argc) {
        opt->files = (const char**)&argv[optind];
    }

    try {
        Controller c(opt);

        if (listDrivers) {
            for (UxController* controller : g_container.uxControllers) {
                printf("\t%s\n", controller->getName());
            }
            return 0;
        }

        c.run();
    } catch (std::exception& e) {
        fprintf(stderr, "%s\n", e.what());
    }

    return 0;
}
