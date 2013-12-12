/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <linux/input.h>

#include "clc/support/Debug.h"
#include "clc/support/Logger.h"
#include "ocher/device/kobo/KoboEvents.h"

#define LOG_NAME "ocher.dev.kobo"


/*
Processor       : ARMv7 Processor rev 5 (v7l)
BogoMIPS        : 159.90
Features        : swp half thumb fastmult vfp edsp neon vfpv3 
CPU implementer : 0x41
CPU architecture: 7
CPU variant     : 0x2
CPU part        : 0xc08
CPU revision    : 5

Hardware        : Freescale MX50 ARM2 Board
Revision        : 50011
Serial          : 0000000000000000
*/


struct KoboButtonEvent {
	uint16_t timeHigh;
	uint16_t res1;
	uint16_t timeLow;
	uint16_t res2;
	uint16_t res3;
	uint16_t button;
	uint16_t press;
	uint16_t res4;
};

KoboEvents::KoboEvents() :
	m_loop(0)
{
	m_buttonFd = open("/dev/input/event0", O_RDONLY | O_NONBLOCK);
	m_touchFd = open("/dev/input/event1", O_RDONLY | O_NONBLOCK);
}

KoboEvents::~KoboEvents()
{
	if (m_buttonFd != -1) {
		close(m_buttonFd);
	}
	if (m_touchFd != -1) {
		close(m_touchFd);
	}
}

void KoboEvents::setEventLoop(EventLoop* loop)
{
	m_loop = loop;

	if (m_buttonFd != -1) {
		ev_io_init(&m_buttonWatcher, buttonCb, m_buttonFd, EV_READ);
		ev_io_start(m_loop->evLoop, &m_buttonWatcher);
		m_buttonWatcher.data = this;
	}
	if (m_touchFd != -1) {
		ev_io_init(&m_touchWatcher, touchCb, m_touchFd, EV_READ);
		ev_io_start(m_loop->evLoop, &m_touchWatcher);
		m_touchWatcher.data = this;
	}
}

void KoboEvents::buttonCb(struct ev_loop*, ev_io* watcher, int)
{
	((KoboEvents*)watcher->data)->pollButton();
}

void KoboEvents::pollButton()
{
	while (1) {
		struct KoboButtonEvent kbe;
		int r = read(m_buttonFd, &kbe, sizeof(kbe));
		if (r == -1) {
			if (errno == EINTR)
				continue;
			ASSERT(errno == EAGAIN || errno == EWOULDBLOCK);
			break;
		} else if (r == sizeof(kbe)) {
			bool fire = true;
			OcherKeyEvent evt;
			clc::Log::debug(LOG_NAME, "button type %x", kbe.button);
			if (kbe.button == 0x66) {
				evt.subtype = kbe.press ? OEVT_KEY_DOWN : OEVT_KEY_UP;
				evt.key = OEVTK_HOME;
			} else if (kbe.button == 0x74) {
				evt.subtype = kbe.press ? OEVT_KEY_DOWN : OEVT_KEY_UP;
				evt.key = OEVTK_POWER;
			} else {
				fire = false;
			}
			if (fire) {
				m_loop->keyEvent(&evt);
			}
		} else {
			break;
		}
	}
}

void KoboEvents::touchCb(struct ev_loop*, ev_io* watcher, int)
{
	((KoboEvents*)watcher->data)->pollTouch();
}

void KoboEvents::pollTouch()
{
	// If this is not enough, libev will call again.  Simple is good.
	struct input_event kevt[64];

	int r;
	do {
		r = read(m_touchFd, kevt, sizeof(kevt));
	} while (r == -1 && errno == EINTR);

	unsigned int n = r / sizeof(struct input_event);
	clc::Log::debug(LOG_NAME, "read %u events", n);

	// http://www.kernel.org/doc/Documentation/input/event-codes.txt
	int syn = 0;
	for (unsigned int i = 0; i < n; ++i) {
		clc::Log::debug(LOG_NAME, "type %d code %d value %d", kevt[i].type, kevt[i].code,
				kevt[i].value);
		if (kevt[i].type == EV_SYN) {
			syn = 1;
		} else if (kevt[i].type == EV_ABS) {
			uint16_t code = kevt[i].code;
			if (code == ABS_X) {
				m_evt.x = kevt[i].value;
			} else if (code == ABS_Y) {
				m_evt.y = kevt[i].value;
			} else if (code == ABS_PRESSURE) {
				unsigned int pressure = kevt[i].value;
				if (pressure == 0)
					m_evt.subtype = OEVT_MOUSE1_UP;
				else {
					// TODO: 100 down, 101 drag
					m_evt.subtype = OEVT_MOUSE1_DOWN;
				}
			}
		}

		if (syn) {
#if 1  // TODO: Why do down vs up coordinate systems differ??
			if (m_evt.subtype == OEVT_MOUSE1_DOWN) {
				m_evt.x = 600 - m_evt.y;
				m_evt.y = m_evt.x;
			}
#endif
			clc::Log::info(LOG_NAME, "mouse %u, %u %s", m_evt.x, m_evt.y,
					m_evt.subtype == OEVT_MOUSE1_DOWN ? "down" : "up");
			m_loop->mouseEvent(&m_evt);
		}
	}
}
