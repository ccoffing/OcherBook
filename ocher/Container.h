/*
 * Copyright (c) 2014, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#ifndef OCHER_CONTAINER_H
#define OCHER_CONTAINER_H

#include "clc/data/List.h"

class EventLoop;
class Device;
class Battery;
class Filesystem;
class Settings;
class Options;
class PowerSaver;
class FrameBuffer;
class FontEngine;
class UxController;
class Renderer;

#if 0
template<T>
class Dependency
{
public:
	Dependency(baseName, isSingleton, a);
	const char* baseName;
	bool singleton;
	T* singletonInstance;
	T* allocate() { return m_allocator(); }
	things-to-inject ;
protected:

};


get(T*& t)
{
	lock();
	Dependency<T> d = find dependency;
	if (d->singleton) {
		if (!d->singletonInstance) {
			d->singletonInstance = d->allocate();
			inject;
		}
		t = d->singletonInstance;
	} else {
		t = d->allocate();
		inject;
	}
	unlock();
}

#endif

/**
 * A container for instances.
 *
 * This is trivial dependency injection.  The dependencies are hardcoded.  The Container owns the
 * instances.  Each instance is a singleton.
 */
class Container
{
public:
	Container();
	~Container();

	/**
	 * The list of all user experience drivers.
	 */
	clc::List uxControllers;

	/**
	 * Requires:
	 */
	EventLoop* loop;

	/**
	 * Requires: EventLoop
	 */
	Device* device;

	/**
	 * Requires:
	 */
	Battery* battery;

	/**
	 * Requires:
	 */
	Filesystem* filesystem;

	/**
	 * Requires: Filesystem
	 */
	Settings* settings;

	Options* options;

	/**
	 * Requires: EventLoop Device
	 */
	PowerSaver* powerSaver;

	/**
	 */
	UxController* uxController;

	/**
	 * May not exist, depending on Device.
	 * May not actually be used, depending on UxController.
	 *
	 * Requires:
	 */
	FrameBuffer* frameBuffer;

	/**
	 * May not exist, depending on Device.
	 * May not actually be used, depending on UxController.
	 *
	 * Requires:
	 * 	- Framebuffer
	 */
	FontEngine* fontEngine;

	/**
	 * Requires:
	 * 	- UxController
	 */
	Renderer* renderer;

	// TODO event source(s) (feeds to EventLoop.  eg: SdlThread, KoboEvents, BrowseFd.cpp:getKey, ncurses, ...)
	// TODO time (common timebase for events.  needed by event loop, clock, ...)
};

extern Container g_container;

#endif
