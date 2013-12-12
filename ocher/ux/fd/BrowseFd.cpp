/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#include <sys/ioctl.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>

#include "ocher/device/Filesystem.h"
#include "ocher/settings/Options.h"
#include "ocher/shelf/Meta.h"
#include "ocher/ux/fd/BrowseFd.h"
#include "ocher/ux/Factory.h"

// TODO:  handle !isatty(stdin)

static char getKey()
{
	int key;
	struct termios oldTermios, newTermios;

	tcgetattr(0, &oldTermios);
	newTermios = oldTermios;
	newTermios.c_lflag &= ~(ICANON | ECHO);
	newTermios.c_cc[VTIME] = 0;
	newTermios.c_cc[VMIN] = 1;
	tcsetattr(0, TCSANOW, &newTermios);

	key = getc(stdin);

	tcsetattr(0, TCSANOW, &oldTermios);

	return (char)key;
}

BrowseFd::BrowseFd()
{
}

bool BrowseFd::init()
{
	m_in = opt.inFd;
	m_out = opt.outFd;
	return true;
}

Meta* BrowseFd::browse(clc::List* meta)
{
	printf("\n");
	for (unsigned int i = 0; i < meta->size(); ++i) {
		Meta* m = (Meta*)meta->get(i);
		// TODO:  title/author
		// TODO:  pad to width
		printf("%3d: %s\n", i+1, m->title.c_str());
		printf("     %s\n", m->author.c_str());
		printf("     %4s  %u of %d\n", Meta::fmtToStr(m->format), 0, 100);
	}

	// TODO temp
	char key = getKey();
	return (Meta*)meta->get(key - '1');
}

void BrowseFd::read(Meta* meta)
{
	Renderer* renderer = uiFactory->getRenderer();
	for (int pageNum = 0; ; ) {
		int atEnd = renderer->render(&meta->m_pagination, pageNum, true);

		char buf[16];
		sprintf(buf, "%u of %u: ", pageNum+1, meta->m_pagination.numPages()+1);
		write(m_out, buf, strlen(buf));

		char key = getKey();
		if (key == 'p' || key == 'b') {
			if (pageNum > 0)
				pageNum--;
		} else if (key == 'q' || atEnd) {
			break;
		} else {
			pageNum++;
		}
	}
}

