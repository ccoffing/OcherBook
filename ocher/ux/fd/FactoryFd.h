/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#ifndef OCHER_UX_FACTORY_FD_H
#define OCHER_UX_FACTORY_FD_H

#include "ocher/ux/Factory.h"
#include "ocher/ux/fd/RenderFd.h"


class UiFactoryFd : public UiFactory
{
public:
	UiFactoryFd();
	virtual ~UiFactoryFd() {}

	bool init();
	void deinit();
	const char* getName();
	Renderer* getRenderer();

protected:
	RendererFd* m_renderer;
};

#endif


