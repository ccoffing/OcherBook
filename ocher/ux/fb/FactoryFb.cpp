/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#include "ocher/ux/fb/FrameBuffer.h"
#include "ocher/ux/fb/FactoryFb.h"
#include "ocher/ocher.h"


UiFactoryFb::UiFactoryFb() :
	m_fb(0),
	m_ft(0),
	m_render(0)
{
}

bool UiFactoryFb::init()
{
	// Derived init must have set m_fb.
	m_ft = new FreeType(m_fb->dpi());
	if (m_ft->init()) {
		m_render = new RenderFb(m_fb);
		if (m_render->init()) {
			return true;
		}
	}
	deinit();
	return false;
}

void UiFactoryFb::deinit()
{
}

