#include "ArticleRequestScreen.h"

#include <WiFi.h>

#include "WikipediaApplication.h"

ArticleRequestScreen::ArticleRequestScreen(WikipediaApplication& application)
    : Screen(application)
{}

void ArticleRequestScreen::setup()
{
    ExtendedMinitelPtr& minitel = application.getMinitel();
    minitel->displayNewPage("3615 Wikipedia");
    minitel->displayString("Loading article...", 1, 3);
}

bool ArticleRequestScreen::update(unsigned long key)
{
    if (WiFi.status() != WL_CONNECTED)
    {
        application.setNextScreenId(ScreenId::WifiLoading);
        return false;
    }

    long pageId = application.getChosenPageId();
    application.getClient().fetchSectionList(pageId);
    application.getResultPager().reset(pageId);

    application.setNextScreenId(ScreenId::Result);
    return false;
}
