/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#ifndef OCHER_FILESYSTEM_H
#define OCHER_FILESYSTEM_H

#include "clc/os/Thread.h"

#include "Signals/Signal.h"
using namespace Gallant;


/**
 * Represents the interesting points of the filesystem (namely, where the books exist, and where
 * user settings might be stored.)
 *
 * On some platforms, notifies when the books change.
 */
class Filesystem
{
public:
	Filesystem();
	~Filesystem();

	const char** m_libraries;
	char* m_home;
	char* m_settings;

	void initWatches();
	void deinitWatches();
	void fireEvents();
	Signal2<const char*, const char*> dirChanged;

protected:
	int m_infd;
};

#endif
