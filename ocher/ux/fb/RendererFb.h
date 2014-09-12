/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#ifndef OCHER_FB_RENDER_H
#define OCHER_FB_RENDER_H

#include "ocher/ux/Renderer.h"
#include "ocher/ux/fb/FontEngine.h"

class FrameBuffer;
class Pagination;
class Settings;


class RendererFb : public Renderer
{
public:
	RendererFb(FrameBuffer* fb);

	bool init();
	void deinit();

	int render(Pagination* pagination, unsigned int pageNum, bool doBlit);

protected:
	int outputWrapped(clc::Buffer* b, unsigned int strOffset, bool doBlit);
	void applyAttrs();

	FrameBuffer* m_fb;
	FontEngine m_fe;
	Settings* m_settings;
	int m_penX;
	int m_penY;

	void pushAttrs();
	void applyAttrs(int i);
	void popAttrs();

	Attrs a[10];
	int ai;
};

#endif
