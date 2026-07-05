#include "WikipediaApplication.h"

#include <WiFi.h>

#include "Secrets.h"

const char* WikipediaApplication::ssid = WIFI_SSID;
const char* WikipediaApplication::password = WIFI_PASSWORD;

const unsigned int WikipediaApplication::RESULT_BODY_START_LINE = 3;
const unsigned int WikipediaApplication::RESULT_BODY_END_LINE = maxPositionY;

void WikipediaApplication::setup()
{
    minitel.defaultSetup();

    currentStep = Step::WifiLoading;
    minitel.displayNewPage("Loading...");

    connectToWifi();

    showPromptPage();
}

void WikipediaApplication::loop()
{
    switch (currentStep)
    {
    case Step::UserInput:
        handleUserInputStep();
        break;

    case Step::HttpRequest:
        handleHttpRequestStep();
        break;

    case Step::DisplayResult:
        handleDisplayResultStep();
        break;

    case Step::WifiLoading:
    default:
        break;
    }
}

void WikipediaApplication::connectToWifi()
{
    minitel.print("Connection to Wi-fi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        minitel.print(".");
    }
    minitel.println("DONE");
}

void WikipediaApplication::showPromptPage()
{
    currentStep = Step::UserInput;

    stringEnteredByUser = "";

    minitel.displayNewPage("3615 Wikipedia");
    minitel.println("What are you searching for?");
    minitel.cursor();
}

void WikipediaApplication::showResultPage(int pageIndex)
{
    minitel.displayNewPage("3615 Wikipedia");

    minitel.attributs(CARACTERE_ROUGE);
    minitel.println("Result for \"" + stringEnteredByUser + "\":");
    minitel.attributs(CARACTERE_BLANC);

    if (resultPager.isEmpty())
    {
        minitel.println("We found nothing :(");
    }
    else
    {
        resultPager.displayPage(minitel, pageIndex, RESULT_BODY_START_LINE, RESULT_BODY_END_LINE);
    }

    currentStep = Step::DisplayResult;
}

void WikipediaApplication::handleUserInputStep()
{
    unsigned long key = minitel.updatePressedKey();
    if (key == 0)
        return;

    switch (key)
    {
    case TOUCHE_FLECHE_GAUCHE:
        break;

    case TOUCHE_FLECHE_DROITE:
        break;

    case ENVOI:
        if (stringEnteredByUser.length() > 0)
        {
            currentStep = Step::HttpRequest;
        }
        break;

    default:
        String keyAsString = minitel.getString(key);
        minitel.print(keyAsString);
        stringEnteredByUser += keyAsString;
        break;
    }
}

void WikipediaApplication::handleHttpRequestStep()
{
    minitel.displayNewPage("3615 Wikipedia");
    minitel.print("Performing search on Wikipedia...");

    if (WiFi.status() == WL_CONNECTED)
    {
        String extract;
        if (wikipediaClient.search(stringEnteredByUser, extract))
        {
            resultPager.reset(extract);
            showResultPage(resultPager.getCurrentPageIndex());
        }
    }
}

void WikipediaApplication::handleDisplayResultStep()
{
    unsigned long key = minitel.updatePressedKey();
    if (key == 0)
        return;

    switch (key)
    {
    case SOMMAIRE:
        showPromptPage();
        break;

    case SUITE:
        showResultPage(resultPager.getCurrentPageIndex() + 1);
        break;

    case RETOUR:
        showResultPage(resultPager.getCurrentPageIndex() - 1);
        break;

    default:
        break;
    }
}
