#include "SearchRequestScreen.h"

#include <WiFi.h>

#include "WikipediaApplication.h"

SearchRequestScreen::SearchRequestScreen(WikipediaApplication& application)
    : Screen(application)
{}

void SearchRequestScreen::setup()
{
    ExtendedMinitelPtr& minitel = application.getMinitel();
    minitel->displayNewPage("3615 Wikipedia");
    minitel->displayString("Searching on Wikipedia...", 1, 3);
}

bool SearchRequestScreen::update(unsigned long key)
{
    if (WiFi.status() != WL_CONNECTED)
    {
        application.setNextScreenId(ScreenId::WifiLoading);
        return false;
    }

    WikipediaClient& client = application.getClient();
    client.search(application.getQuery());

    if (!client.getSearchResults().empty())
    {
        application.setNextScreenId(ScreenId::ResultSelection);
    }
    else if (client.getSuggestion().length() > 0)
    {
        application.setNextScreenId(ScreenId::DidYouMean);
    }
    else
    {
        application.getResultPager().reset(0);
        application.setNextScreenId(ScreenId::Result);
    }

    return false;
}
