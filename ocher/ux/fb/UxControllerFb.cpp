#include "ocher/Container.h"
#include "ocher/ux/fb/RendererFb.h"
#include "ocher/ux/fb/UxControllerFb.h"

#include "clc/support/Logger.h"

#define LOG_NAME "ocher.ux.ctrl"

UxControllerFb::UxControllerFb() :
	m_systemBar(g_container.battery),
	m_activity(0),
	m_bootActivity(this),
	m_homeActivity(this),
	m_libraryActivity(this),
	m_readActivity(this),
	m_settingsActivity(this),
	m_sleepActivity(this),
	m_syncActivity(this),
	m_renderer(0),
	m_fontEngine(0),
	m_frameBuffer(0),
	m_name("fb")
{
}

UxControllerFb::~UxControllerFb()
{
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

	m_frameBuffer = frameBuffer;
	m_renderer = new RendererFb(m_frameBuffer);
	m_fontEngine = new FontEngine(m_frameBuffer);

	return true;
}

void UxControllerFb::setActivity(enum ActivityType a)
{
	clc::Log::info(LOG_NAME, "next activity: %d", a);
#if 0 // TODO
	detachCurrent();

	switch (a) {
		case ACTIVITY_BOOT:
			m_activityPanel = &m_bootActivity;
			break;
		case ACTIVITY_SLEEP:
			m_activityPanel = &m_sleepActivity;
			break;
		case ACTIVITY_SYNC:
			m_activityPanel = &m_syncActivity;
			break;
		case ACTIVITY_HOME:
			m_activityPanel = &m_homeActivity;
			break;
		case ACTIVITY_READ:
			m_activityPanel = &m_readActivity;
			break;
		case ACTIVITY_LIBRARY:
			m_activityPanel = &m_libraryActivity;
			break;
		case ACTIVITY_SETTINGS:
			m_activityPanel = &m_settingsActivity;
			break;
		default:
			ASSERT(0);
	}

	attachCurrent();
#endif
	m_frameBuffer->needFull();
}
