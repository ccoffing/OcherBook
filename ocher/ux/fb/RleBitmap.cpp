/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#include "ocher/ux/fb/RleBitmap.h"


RleBitmap::RleBitmap() :
	m_p(0)
{
}

void RleBitmap::pack(uint8_t* u, unsigned int len, uint8_t* p)
{
	unsigned int count = 0;
	uint8_t runOf;
	for (unsigned int i = 0; i < len; ++i) {
		if (! count) {
			runOf = u[i];
			++count;
		} else if (runOf != u[i] || count == 257) {
			*p++ = runOf;
			if (--count) {
				*p++ = runOf;
				*p++ = --count;
				count = 0;
			}
			runOf = u[i];
		} else {
			++count;
		}
	}
	if (count) {
		*p++ = runOf;
		if (--count) {
			*p++ = runOf;
			*p++ = --count;
		}
	}
}

int RleBitmap::unpack(uint8_t* u, unsigned int len)
{
	unsigned int count = 0;
	uint8_t runOf;
	for (unsigned int i = 0; i < len && m_off < m_len; ) {
		if (! count) {
			runOf = m_p[m_off++];
			++count;
		} else if (count == 1) {
			if (runOf != m_p[m_off++]) {

			}



		}
	}
	return 0;
}
