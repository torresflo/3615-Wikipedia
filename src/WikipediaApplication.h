#ifndef WIKIPEDIAAPPLICATION_H
#define WIKIPEDIAAPPLICATION_H

#include <memory>

#include <MinitelToolkit.h>

#include "WikipediaClient.h"
#include "WikipediaResultPager.h"

#include "Pages/Screen.h"
#include "Pages/WifiLoadingScreen.h"
#include "Pages/WelcomeScreen.h"
#include "Pages/SearchRequestScreen.h"
#include "Pages/ResultSelectionScreen.h"
#include "Pages/DidYouMeanScreen.h"
#include "Pages/ArticleRequestScreen.h"
#include "Pages/ResultScreen.h"
#include "Pages/SectionListScreen.h"

class WikipediaApplication
{
public:
    explicit WikipediaApplication();
    void setup();
    void loop();

    void setNextScreenId(ScreenId id);

    ExtendedMinitelPtr& getMinitel();
    WikipediaClient& getClient();
    WikipediaResultPager& getResultPager();

    const String& getQuery() const;
    void setQuery(String value);

    long getChosenPageId() const;
    void setChosenPageId(long value);

private:
    Screen* screenFor(ScreenId id);
    void switchTo(ScreenId id);

    ExtendedMinitelPtr minitel = std::make_shared<ExtendedMinitel>();
    WikipediaClient wikipediaClient;
    WikipediaResultPager resultPager;

    WifiLoadingScreen wifiLoadingScreen;
    WelcomeScreen welcomeScreen;
    SearchRequestScreen searchRequestScreen;
    ResultSelectionScreen resultSelectionScreen;
    DidYouMeanScreen didYouMeanScreen;
    ArticleRequestScreen articleRequestScreen;
    ResultScreen resultScreen;
    SectionListScreen sectionListScreen;

    Screen* currentScreen = nullptr;
    ScreenId nextScreenId = ScreenId::Welcome;

    String query;
    long chosenPageId = 0;
};

#endif
