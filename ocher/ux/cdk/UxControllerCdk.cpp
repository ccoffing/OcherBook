#include "ocher/ux/cdk/UxControllerCdk.h"

UxControllerCdk::UxControllerCdk() :
	m_scr(0),
	m_screen(0),
	m_renderer(0)
{
}

UxControllerCdk::~UxControllerCdk()
{
	delete m_renderer;
	m_renderer = 0;

	destroyCDKScreen(m_screen);
	endCDK();
	delwin(m_scr);
}

UxControllerCdk::init()
{
	m_renderer = new RendererCdk();

	m_scr = initscr();
	m_screen = initCDKScreen(m_scr);
	initCDKColor();
	m_renderer->init(m_scr, m_screen);

}

void UxControllerCdk::run(enum ActivityType a)
{
	// TODO
}
