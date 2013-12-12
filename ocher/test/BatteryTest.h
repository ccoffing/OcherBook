/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#include "ocher/device/Battery.h"

SUITE(Battery)
{
	TEST(Capacity)
	{
		PRINT_PROGRESS;

		Battery b;
		int r;

		r = b.readCapacity();
		if (r == -1) {
			CHECK_EQUAL(-1, b.m_percent);
		} else {
			CHECK(b.m_percent >= 0) ;
			CHECK(b.m_percent <= 100);
		}
	}

	TEST(Status)
	{
		PRINT_PROGRESS;

		Battery b;
		int r;

		r = b.readStatus();
		if (r == -1) {
			CHECK_EQUAL(Battery::Unknown, b.m_status);
		} else {
			CHECK(b.m_status == Battery::Charging || b.m_status == Battery::Discharging);
		}
	}
}

