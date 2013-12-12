/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#ifndef OCHER_UX_CONTROLLER_H
#define OCHER_UX_CONTROLLER_H

#include "ocher/ux/HomeActivity.h"
#include "ocher/ux/LibraryActivity.h"
#include "ocher/ux/ReadActivity.h"
#include "ocher/ux/SettingsActivity.h"
#include "ocher/ux/SyncActivity.h"
#include "ocher/ux/BootActivity.h"
#include "ocher/shelf/Shelf.h"

class PowerSaver;

class Context
{
public:
	Context() : shortList(&library), selected(0) {}
	Library library;
	ShortList shortList;

	Meta* selected;
};

class Controller
{
public:
	Controller();
	~Controller();

	void onDirChanged(const char* dir, const char* file);
	void onWantToSleep();
	void onAppEvent(struct OcherAppEvent* evt);

	void run(Activity a);
	void setNextActivity(Activity a);

	Context ctx;
	UiBits ui;

protected:
	void detachCurrent();
	void attachCurrent();

	Screen m_screen;
	Activity m_activity;
	Window* m_activityPanel;

	HomeActivity m_homeActivity;
	LibraryActivity m_libraryActivity;
	ReadActivity m_readActivity;
	SettingsActivity m_settingsActivity;
	SyncActivity m_syncActivity;
	BootActivity m_bootActivity;
	PowerSaver* m_powerSaver;
};

#endif

