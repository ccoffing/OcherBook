/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#ifndef OCHER_OPTIONS_H
#define OCHER_OPTIONS_H

/**
 * Options set per program run
 */
class Options {
public:
	Options() :
		verbose(0),
		bootMenu(false),
		driverName(0),
		files(0),
		watches(0),
		inFd(0),
		outFd(1) {}

	int verbose;

	/**
	 * Start at boot menu?
	 */
	bool bootMenu;

	const char* driverName;

	/** Files and/or directories passed on the command line, to be synced once.
	 */
	const char** files;

	/** @todo directories to watch
	 */
	const char** watches;

	int inFd;
	int outFd;
};

#endif

