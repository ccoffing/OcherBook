/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#ifndef OCHER_UX_FACTORY_H
#define OCHER_UX_FACTORY_H

#include "clc/data/List.h"

#include "ocher/shelf/Shelf.h"
#include "ocher/ux/Renderer.h"
#include "ocher/ux/fb/FrameBuffer.h"
#include "ocher/ux/fb/FreeType.h"


#define UX_DRIVER_REGISTER(driver) \
	class Register##driver { \
	public: \
		Register##driver() { getDrivers().add(new UiFactory##driver); } \
	} reg##driver

class EventLoop;

/**
 * The list of all user experience drivers.  All compiled-in drivers automatically register
 * themselves here.
 */
clc::List& getDrivers();

extern EventLoop* g_loop;
extern FrameBuffer* g_fb;
extern FreeType* g_ft;

class UiFactory
{
public:
	UiFactory();
	virtual ~UiFactory() {}

	/**
	 * Called after user options have been parsed.
	 * @return True iff sucessfully initialized, else output msg
	 */
	virtual bool init() = 0;

	void populate();

	virtual void deinit() = 0;

	virtual const char* getName() = 0;

	virtual Renderer* getRenderer() = 0;

	virtual FrameBuffer* getFrameBuffer() { return (FrameBuffer*)0; }

	virtual FreeType* getFontEngine() { return (FreeType*)0; }
};

extern UiFactory* uiFactory;

#endif
