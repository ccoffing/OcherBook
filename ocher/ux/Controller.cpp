/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#include "ocher/Container.h"
#include "ocher/device/Battery.h"
#include "ocher/device/Device.h"
#include "ocher/device/Filesystem.h"
#include "ocher/settings/Options.h"
#include "ocher/settings/Settings.h"
#include "ocher/ux/Controller.h"
#include "ocher/ux/PowerSaver.h"
#ifdef UX_FB
#include "ocher/ux/fb/UxControllerFb.h"
#endif
#ifdef UX_CDK
#include "ocher/ux/cdk/UxControllerCdk.h"
#endif
#ifdef UX_FD
#include "ocher/ux/fd/UxControllerFd.h"
#endif

#include "airbag_fd/airbag_fd.h"
#include "clc/support/Debug.h"
#include "clc/support/LogAppenders.h"
#include "clc/support/Logger.h"

#define LOG_NAME "ocher.controller"


UxController::UxController() :
	m_powerSaver(0),
	m_loop(0)
{
	m_filesystem = g_container.filesystem;
	m_loop = g_container.loop;
	m_powerSaver = g_container.powerSaver;

	m_filesystem->dirChanged.Connect(this, &UxController::onDirChanged);
	m_filesystem->initWatches();

	m_loop->appEvent.Connect(this, &UxController::onAppEvent);
	m_powerSaver->wantToSleep.Connect(this, &UxController::onWantToSleep);
}

UxController::~UxController()
{
	m_filesystem->dirChanged.Disconnect(this, &UxController::onDirChanged);
	m_loop->appEvent.Disconnect(this, &UxController::onAppEvent);
	m_powerSaver->wantToSleep.Disconnect(this, &UxController::onWantToSleep);
}

void UxController::onDirChanged(const char* dir, const char* file)
{
	clc::Log::info(LOG_NAME, "onDirChanged %s %s", dir, file);
	// TODO
}

void UxController::onWantToSleep()
{
	clc::Log::info(LOG_NAME, "onWantToSleep");

	// TODO
}

void UxController::onAppEvent(struct OcherAppEvent* evt)
{
	if (evt->subtype == OEVT_APP_CLOSE) {
		m_nextActivity = ACTIVITY_QUIT;
		m_loop->stop();
	}
}

Controller::Controller(Options* options) :
	m_uxController(0)
{
	g_container.options = options;
	initLog();

	g_container.settings = new Settings();
	g_container.filesystem = new Filesystem();
	g_container.powerSaver = new PowerSaver();
	g_container.device = new Device();
	g_container.battery = new Battery();
	g_container.loop = new EventLoop();

	// Wire up
	//   TODO setters vs constructors vs hitting g_container
	//   TODO here or in Container?
	g_container.settings->inject(g_container.filesystem);
	g_container.powerSaver->inject(g_container.device);
	g_container.powerSaver->inject(g_container.loop);

	initDebug();

	// Sort UxController list based on desirability
	// (assume native >> toolkits >> framebuffer >> tui >> text)
#ifdef UX_FB
	g_container.uxControllers.add(new UxControllerFb);
#endif
#ifdef UX_CDK
	g_container.uxControllers.add(new UxControllerCdk);
#endif
#ifdef UX_FD
	g_container.uxControllers.add(new UxControllerFd);
#endif

	// TODO....  move to run() or init()

	UxController* uxController = NULL;
	for (unsigned int i = 0; i < g_container.uxControllers.size(); ++i) {
		UxController* c = (UxController*)g_container.uxControllers.get(i);
		clc::Log::info("ocher", "considering driver %s", c->getName());

		if (options->driverName) {
			if (strcmp(c->getName(), options->driverName) == 0) {
				clc::Log::debug("ocher", "Attempting to init the '%s' driver", options->driverName);
				if (!c->init()) {
					clc::Log::warn("ocher", "Failed to init the '%s' driver", options->driverName);
					throw "failed to init driver"; // TODO
				}
				uxController = c;
				break;
			}
		} else {
			if (c->init() == true) {
				uxController = c;
				break;
			} else {
				clc::Log::info("ocher", "driver %s failed to init", c->getName());
			}
		}
	}

	if (uxController == NULL) {
		throw "failed to find suitable output driver"; // TODO
	}
	clc::Log::info("ocher", "Using the '%s' driver", uxController->getName());

	g_container.uxController = uxController;
#ifdef UX_FB
	g_container.frameBuffer = uxController->getFrameBuffer();
	g_container.frameBuffer->inject(g_container.loop);
#endif
	g_container.fontEngine = uxController->getFontEngine();
	g_container.renderer = uxController->getRenderer();

	m_uxController = g_container.uxController;

	initCrash();
}

Controller::~Controller()
{
}

void Controller::initCrash()
{
	// TODO:  Could pass on to m_uxController...
#ifdef OCHER_AIRBAG_FD
	airbag_init_fd(2, 0);
#endif
}

void Controller::initLog()
{
	static clc::LogAppenderCFile appender(stderr);
	clc::Logger* l = clc::Log::get("");
	l->setAppender(&appender);

	Options* opt = g_container.options;
	if (opt->verbose < 0)
		l->setLevel(clc::Log::Fatal);
	else if (opt->verbose == 0)
		l->setLevel(clc::Log::Error);
	else if (opt->verbose == 1)
		l->setLevel(clc::Log::Warn);
	else if (opt->verbose == 2)
		l->setLevel(clc::Log::Info);
	else if (opt->verbose == 3)
		l->setLevel(clc::Log::Debug);
	else
		l->setLevel(clc::Log::Trace);
	clc::Log::info("ocher", "Log initialized");
}

void Controller::initDebug()
{
#if 0
	// Before proceeding with startup and initializing the framebuffer, check for a killswitch.
	// Useful when needing to bail to native stack (such as OcherBook vs native stack init-ing
	// framebuffer in incompatible ways).
	if (g_container.device->fs.m_libraries) {
		for (int i = 0; ; ++i) {
			const char* lib = g_container.device->fs.m_libraries[i];
			if (! lib)
				break;
			clc::Buffer killswitch(1, "%s/.ocher/kill", lib);
			if (access(killswitch.c_str(), F_OK) == 0) {
				fprintf(stderr, "OcherBook exiting because of '%s' killswitch\n", killswitch.c_str());
				exit(1);
			}
		}
	}
#endif
}

void Controller::run()
{
#ifdef OCHER_TARGET_KOBO
	// Kobo rc scripts start animate.sh, which shows an animation while nickel is starting.
	// Kill that here (so it doesn't overlay the boot menu) to simplify installation steps.
	// TODO: Remove when more closely integrated.
	system("killall on-animator.sh");
	sleep(1);
#endif

	Options* opt = g_container.options;
	ActivityType a = opt->bootMenu ? ACTIVITY_BOOT : ACTIVITY_SYNC;
	m_uxController->run(a);

	// TODO: sync state out
}
