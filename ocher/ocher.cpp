/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#include "ocher/settings/Options.h"
#include "ocher/ux/Controller.h"
#include "ocher/Container.h"

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <exception>


void usage(const char* msg)
{
	printf(
		"OcherBook  Copyright (C) 2012 Chuck Coffing  <clc@alum.mit.edu>\n"
		"Redistribution permitted under BSD 2-clause license\n"
		"\n");
	if (msg) {
		printf("%s\n\n", msg);
	}
	printf(
	      //12345678901234567890123456789012345678901234567890123456789012345678901234567890
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
	Options* opt = new Options;

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

		int ch = getopt_long(argc, argv, "d:bfhtvq", long_options, &option_index);
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
				usage(0);
				break;
			case OPT_DRIVER:
				opt->driverName = optarg;
				break;
			case OPT_LIST_DRIVERS:
				listDrivers = true;
				break;
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
			for (unsigned int i = 0; i < g_container.uxControllers.size(); ++i) {
				UxController* controller = (UxController*)g_container.uxControllers.get(i);
				printf("\t%s\n", controller->getName());
			}
			return 0;
		}

		c.run();
	} catch (std::exception &e) {
		fprintf(stderr, "%s\n", e.what());
	}

	return 0;
}
