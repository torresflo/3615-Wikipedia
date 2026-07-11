#include "WikipediaApplication.h"

#include <utility>

WikipediaApplication::WikipediaApplication()
    : resultPager(wikipediaClient)
    , wifiLoadingScreen(*this)
    , welcomeScreen(*this)
    , searchRequestScreen(*this)
    , resultSelectionScreen(*this)
    , didYouMeanScreen(*this)
    , articleRequestScreen(*this)
    , resultScreen(*this)
    , sectionListScreen(*this)
{}

ExtendedMinitelPtr& WikipediaApplication::getMinitel()
{
    return minitel;
}

WikipediaClient& WikipediaApplication::getClient()
{
    return wikipediaClient;
}

WikipediaResultPager& WikipediaApplication::getResultPager()
{
    return resultPager;
}

const String& WikipediaApplication::getQuery() const
{
    return query;
}

void WikipediaApplication::setQuery(String value)
{
    query = std::move(value);
}

long WikipediaApplication::getChosenPageId() const
{
    return chosenPageId;
}

void WikipediaApplication::setChosenPageId(long value)
{
    chosenPageId = value;
}

void WikipediaApplication::setup()
{
    minitel->defaultSetup();
    debugBegin();

    switchTo(ScreenId::WifiLoading);
}

void WikipediaApplication::loop()
{
    unsigned long key = minitel->updatePressedKey();

    switch(key)
    {
    case SOMMAIRE:
        setNextScreenId(ScreenId::Welcome);
        switchTo(nextScreenId);
        return;
    }

    if (!currentScreen->update(key))
        switchTo(nextScreenId);
}

void WikipediaApplication::setNextScreenId(ScreenId id)
{
    nextScreenId = id;
}

Screen* WikipediaApplication::screenFor(ScreenId id)
{
    switch (id)
    {
    case ScreenId::WifiLoading:
        return &wifiLoadingScreen;
    case ScreenId::Welcome:
        return &welcomeScreen;
    case ScreenId::SearchRequest:
        return &searchRequestScreen;
    case ScreenId::ResultSelection:
        return &resultSelectionScreen;
    case ScreenId::DidYouMean:
        return &didYouMeanScreen;
    case ScreenId::ArticleRequest:
        return &articleRequestScreen;
    case ScreenId::Result:
        return &resultScreen;
    case ScreenId::SectionList:
        return &sectionListScreen;
    }

    return &welcomeScreen;
}

void WikipediaApplication::switchTo(ScreenId id)
{
    currentScreen = screenFor(id);
    currentScreen->setup();
}
