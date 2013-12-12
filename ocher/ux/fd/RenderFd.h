/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#ifndef OCHER_FD_RENDERER_H
#define OCHER_FD_RENDERER_H

#include "ocher/ux/Renderer.h"


class RendererFd : public Renderer
{
public:
	RendererFd();

	bool init();
	int render(Pagination* pagination, unsigned int pageNum, bool doBlit);

	int outputWrapped(clc::Buffer* b, unsigned int strOffset, bool doBlit);

protected:
	int m_isTty;

	int m_fd;
	int m_width;
	int m_height;
	int m_x;
	int m_y;

	void clearScreen();
	void enableUl();
	void disableUl();
	void enableEm();
	void disableEm();

	void pushAttrs();
	void applyAttrs(int i);
	void popAttrs();

	Attrs a[10];
	int ai;
};

#endif
