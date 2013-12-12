/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#include <stdio.h>
#include <UnitTest++.h>

#define CLC_LOG_LEVEL 5
#include "clc/support/LogAppenders.h"

#include "ocher/settings/Options.h"
#include "ocher/device/Device.h"
#include "ocher/ux/Factory.h"

#define PRINT_PROGRESS  printf("Testing %s::%s...\n", UnitTestSuite::GetSuiteName(), m_details.testName);fflush(stdout)

#define REQUIRE_RESOURCES_DIR \
	if (! resourcesDir) { \
		printf("\tSpecify resources directory on command line; skipping test...\n"); \
		return; \
	}

void resetUnitTestLoggers()
{
	static clc::LogAppenderCFile console(stderr);
	clc::Log::reset();
	clc::Log::get("")->setLevel(clc::Log::Warn);
	clc::Log::get("")->setAppender(&console);
}

char* resourcesDir;
struct Options opt;
UiFactory* uiFactory;

#include "BatteryTest.h"
//#include "ShelfTest.h"  // TODO test searching
#include "FormatTest.h"
#include "EpubTest.h"

int main(int /*argc*/, char** argv)
{
	resetUnitTestLoggers();
	resourcesDir = argv[1];
	initDevice();
	uiFactory = (UiFactory*)drivers.get(0);
	uiFactory->init();
	uiFactory->populate();
	int r = UnitTest::RunAllTests();
	uiFactory->deinit();
	return r;
}
