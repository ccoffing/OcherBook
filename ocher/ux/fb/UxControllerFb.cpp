#include "ocher/Container.h"
#include "ocher/ux/fb/RendererFb.h"
#include "ocher/ux/fb/UxControllerFb.h"
#ifdef UX_FB_SDL
#include "ocher/ux/fb/sdl/FrameBufferSdl.h"
#endif

#include "clc/support/Logger.h"

#define LOG_NAME "ocher.ux.ctrl"

UxControllerFb::UxControllerFb() :
	m_systemBar(0),
	m_activity(0),
	m_bootActivity(0),
	m_homeActivity(0),
	m_libraryActivity(0),
	m_readActivity(0),
	m_settingsActivity(0),
	m_sleepActivity(0),
	m_syncActivity(0),
	m_renderer(0),
	m_fontEngine(0),
	m_frameBuffer(0),
	m_name("fb")
{
}

UxControllerFb::~UxControllerFb()
{
	delete m_bootActivity;
	delete m_homeActivity;
	delete m_libraryActivity;
	delete m_readActivity;
	delete m_settingsActivity;
	delete m_sleepActivity;
	delete m_syncActivity;

	delete m_systemBar;
	delete m_navBar;

	delete m_renderer;
	delete m_fontEngine;
	delete m_frameBuffer;

}

bool UxControllerFb::init()
{
	FrameBuffer* frameBuffer;

	do {
#ifdef UX_FB_SDL
		frameBuffer = new FrameBufferSdl();
		if (frameBuffer->init()) {
			m_name += ".sdl";
			break;
		}
#endif
#ifdef OCHER_TARGET_KOBO
		frameBuffer = new FrameBufferMx50();
		if (frameBuffer->init()) {
			m_name += ".mx50";
			break;
		}
#endif
		return false;
	} while (0);

	g_container.frameBuffer = m_frameBuffer = frameBuffer;
	m_renderer = new RendererFb(m_frameBuffer);
	m_fontEngine = new FontEngine(m_frameBuffer);

	m_systemBar = new SystemBar(m_frameBuffer, g_container.battery);
	m_navBar = new NavBar(m_frameBuffer);

	m_bootActivity = new BootActivityFb(this);
	m_homeActivity = new HomeActivityFb(this);
	m_libraryActivity = new LibraryActivityFb(this);
	m_readActivity = new ReadActivityFb(this);
	m_settingsActivity = new SettingsActivityFb(this);
	m_sleepActivity = new SleepActivityFb(this);
	m_syncActivity = new SyncActivityFb(this);

	return true;
}

void UxControllerFb::setNextActivity(enum ActivityType a)
{
	m_nextActivity = a;
}

void UxControllerFb::run(enum ActivityType a)
{
	setNextActivity(a);

	while (m_nextActivity != ACTIVITY_QUIT) {
		clc::Log::info(LOG_NAME, "next activity: %d", m_nextActivity);

		if (m_activity) {
			m_screen->removeChild(m_activity);
			m_activity->onDetached();
		}

		switch (m_nextActivity) {
		case ACTIVITY_BOOT:
			m_activity = m_bootActivity;
			break;
		case ACTIVITY_SLEEP:
			m_activity = m_sleepActivity;
			break;
		case ACTIVITY_SYNC:
			m_activity = m_syncActivity;
			break;
		case ACTIVITY_HOME:
			m_activity = m_homeActivity;
			break;
		case ACTIVITY_READ:
			m_activity = m_readActivity;
			break;
		case ACTIVITY_LIBRARY:
			m_activity = m_libraryActivity;
			break;
		case ACTIVITY_SETTINGS:
			m_activity = m_settingsActivity;
			break;
		default:
			ASSERT(0);
		}

		m_screen->addChild(m_activity);
		m_activity->onAttached();

		m_frameBuffer->needFull();
	}
}
