#ifndef WIKIPEDIAAPPLICATION_H
#define WIKIPEDIAAPPLICATION_H

#include <MinitelToolkit.h>

#include "WikipediaClient.h"
#include "WikipediaResultPager.h"

class WikipediaApplication
{
public:
    void setup();
    void loop();

private:
    enum class Step
    {
        WifiLoading,
        UserInput,
        HttpRequest,
        DisplayResult
    };

    void connectToWifi();
    void showPromptPage();
    void showResultPage(int pageIndex);

    void handleUserInputStep();
    void handleHttpRequestStep();
    void handleDisplayResultStep();

    static const char* ssid;
    static const char* password;

    static const unsigned int RESULT_BODY_START_LINE;
    static const unsigned int RESULT_BODY_END_LINE;

    ExtendedMinitel minitel;
    WikipediaClient wikipediaClient;
    WikipediaResultPager resultPager;

    Step currentStep = Step::WifiLoading;
    String stringEnteredByUser;
};

#endif
