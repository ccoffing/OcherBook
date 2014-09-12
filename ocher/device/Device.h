/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#ifndef OCHER_DEVICE_H
#define OCHER_DEVICE_H

#include "clc/data/Buffer.h"

/**
 * Represents the physical e-reader device.
 */
class Device
{
public:
	virtual ~Device() {}

	clc::Buffer getVersion();
	clc::Buffer getBuildDate();

	clc::Buffer getMac();
	clc::Buffer getIp();

	virtual void reboot() {}
	virtual void shutdown() {}

	/**
	 * Puts the device in a low power state until ??? TODO.
	 * Returns when awake.
	 */
	virtual void sleep();
};

#endif
