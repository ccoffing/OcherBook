/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#include "clc/storage/File.h"

#include "ocher/device/Device.h"
#include "ocher/settings/Settings.h"

//#include "third-party/jsmn/jsmn.c"

Settings g_settings;


Settings::Settings() :
    trackReading(0),
    encryptReading(1),
    minutesUntilSleep(15),
    sleepShowBook(1),
    minutesUntilPowerOff(60),
    wirelessAirplaneMode(0),
    fullRefreshPages(6),
    showPageNumbers(1),
    fontPoints(10),
    marginTop(10),
    marginBottom(10),
    marginLeft(10),
    marginRight(10),
    smallSpace(10),
    medSpace(15),
    largeSpace(30)
{
#ifdef OCHER_TARGET_KOBO
    fontRoot = "/usr/local/Trolltech/QtEmbedded-4.6.2-arm/lib/fonts";
#else
    // TODO fontRoot = ".:/usr/share/fonts/truetype/ttf-dejavu";
    fontRoot = "/usr/share/fonts/truetype/";
#endif
}

void Settings::load()
{
    clc::File s;
    if (s.setTo(g_device->fs.m_settings) != 0)
        return;

    clc::Buffer line;
    while (!s.isEof()) {
        line = s.readLine(false, 1024);

        const char* p = line.c_str();
        size_t n = line.size();

        // TODO
    }
}

//jsonNumOut(clc::File& f, const char* key, int val)
//{
//    clc::Buffer b;
//    b.format("%s: %d", key, val);
//    f.write(b);
//}

void Settings::save()
{
    clc::File s(g_device->fs.m_settings, "w");

    clc::Buffer b;

    b.format("MinutesUntilSleep=%u\n", minutesUntilSleep);
    s.write(b);
    b.format("MinutesUntilPowerOff=%u\n", minutesUntilPowerOff);
    s.write(b);
    b.format("TrackReading=%u\n", trackReading ? 1 : 0);
    s.write(b);
}
