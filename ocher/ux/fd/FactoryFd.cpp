/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#include "ocher/ux/fd/FactoryFd.h"
#include "ocher/ocher.h"


UX_DRIVER_REGISTER(Fd);


UiFactoryFd::UiFactoryFd() :
	m_renderer(0)
{
}

bool UiFactoryFd::init()
{
	m_renderer = new RendererFd();

	if (m_renderer->init())
		return true;
	deinit();
	return false;
}

void UiFactoryFd::deinit()
{
	delete m_renderer;
	m_renderer = 0;
}

const char* UiFactoryFd::getName()
{
	return "fd";
}

Renderer* UiFactoryFd::getRenderer()
{
	return m_renderer;
}

