#include "WifiLoadingScreen.h"

#include <WiFi.h>

#include "WikipediaApplication.h"
#include "Secrets.h"

const char* WifiLoadingScreen::ssid = WIFI_SSID;
const char* WifiLoadingScreen::password = WIFI_PASSWORD;

WifiLoadingScreen::WifiLoadingScreen(WikipediaApplication& application)
    : Screen(application)
{}

void WifiLoadingScreen::setup()
{
    ExtendedMinitelPtr& minitel = application.getMinitel();
    minitel->displayNewPage("Loading...");
    minitel->displayString("Connection to Wi-fi...", 1, 3);
    minitel->moveCursorXY(1, 2);

    WiFi.begin(ssid, password);
}

bool WifiLoadingScreen::update(unsigned long key)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        application.setNextScreenId(ScreenId::Welcome);
        return false;
    }

    delay(500);
    application.getMinitel()->print(".");
    return true;
}
