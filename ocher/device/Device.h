/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#ifndef OCHER_DEVICE_H
#define OCHER_DEVICE_H

#include "clc/data/Buffer.h"

#include "ocher/device/Filesystem.h"


/**
 * Represents the physical e-reader device.
 */
class Device
{
public:
    clc::Buffer getVersion();
    clc::Buffer getBuildDate();

    clc::Buffer getMac();
    clc::Buffer getIp();

    void reboot() {}

    /**
     * Puts the device in a low power state until ??? TODO.
     * Returns when awake.
     */
    void sleep();

    Filesystem fs;
};

/**
 * The device singleton
 */
extern Device* g_device;

void initDevice();

#endif
