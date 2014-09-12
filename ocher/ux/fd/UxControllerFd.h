/*
 * Copyright (c) 2014, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#ifndef OCHER_UX_FD_UXCONTROLLER_H
#define OCHER_UX_FD_UXCONTROLLER_H

#include "ocher/ux/Controller.h"

/**
 */
class UxControllerFd : public UxController
{
public:
	UxControllerFd();
	~UxControllerFd() {}

	const char* getName() const { return "fd"; }

	bool init();

	Renderer* getRenderer() { return m_renderer; }

	void run(enum ActivityType a);

protected:
	clc::Buffer m_name;

	Renderer* m_renderer;
};

#endif

