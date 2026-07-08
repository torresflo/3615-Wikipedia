#ifndef WIKIPEDIAAPPLICATION_H
#define WIKIPEDIAAPPLICATION_H

#include <memory>

#include <MinitelToolkit.h>

#include "WikipediaClient.h"
#include "WikipediaResultPager.h"

class WikipediaApplication
{
public:
    explicit WikipediaApplication();
    void setup();
    void loop();

private:
    enum class Step
    {
        WifiLoading,
        UserInput,
        SearchRequest,
        ResultSelection,
        DidYouMean,
        ArticleRequest,
        DisplayResult
    };

    void connectToWifi();
    void showPromptPage();
    void showResultSelectionPage();
    void showDidYouMeanPage();
    void showResultPage();
    void showLoadingHint();

    void handleUserInputStep();
    void handleSearchRequestStep();
    void handleResultSelectionStep();
    void handleDidYouMeanStep();
    void handleArticleRequestStep();
    void handleDisplayResultStep();

    static const char* ssid;
    static const char* password;

    ExtendedMinitelPtr minitel = std::make_shared<ExtendedMinitel>();
    WikipediaClient wikipediaClient;
    WikipediaResultPager resultPager;

    TableViewModelPtr searchResultsModel;
    TableViewPtr searchResultsView;

    Step currentStep = Step::WifiLoading;
    String stringEnteredByUser;
    long chosenPageId = 0;
};

#endif
