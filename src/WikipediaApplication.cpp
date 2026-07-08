#include "WikipediaApplication.h"

#include <WiFi.h>

#include "Secrets.h"

const char* WikipediaApplication::ssid = WIFI_SSID;
const char* WikipediaApplication::password = WIFI_PASSWORD;

WikipediaApplication::WikipediaApplication()
    :resultPager(wikipediaClient)
{}

void WikipediaApplication::setup()
{
    minitel.defaultSetup();
    debugBegin();

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
    minitel.displayString("Connection to Wi-fi...", 1, 3);
    minitel.moveCursorXY(1, 2);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        minitel.print(".");
    }
}

void WikipediaApplication::showPromptPage()
{
    currentStep = Step::UserInput;

    stringEnteredByUser = "";

    minitel.displayNewPage("3615 Wikipedia");
    minitel.displayString("What are you searching for?", 1, 3);
    minitel.moveCursorXY(1, 4);
    minitel.cursor();
}

void WikipediaApplication::showResultPage(int pageIndex)
{
    minitel.displayNewPage("3615 Wikipedia");

    if (resultPager.isEmpty())
    {
        minitel.displayString("We found nothing :(", 1, 3);
    }
    else
    {
        minitel.attributs(CARACTERE_ROUGE);
        minitel.displayString("Result for " + stringEnteredByUser + " (page " + String(pageIndex + 1) + "):", 1, 3);
        minitel.attributs(CARACTERE_BLANC);
        resultPager.displayPage(minitel, pageIndex, 5, maxPositionY);
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
    minitel.displayString("Performing search on Wikipedia...", 1, 3);

    if (WiFi.status() == WL_CONNECTED)
    {
        wikipediaClient.search(stringEnteredByUser);
        resultPager.reset();
        showResultPage(resultPager.getCurrentPageIndex());
        currentStep = Step::DisplayResult;
    }
    else
        currentStep = Step::WifiLoading;
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
