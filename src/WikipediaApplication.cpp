#include "WikipediaApplication.h"

#include <vector>

#include <WiFi.h>

#include "Secrets.h"

const char* WikipediaApplication::ssid = WIFI_SSID;
const char* WikipediaApplication::password = WIFI_PASSWORD;

WikipediaApplication::WikipediaApplication()
    :resultPager(wikipediaClient)
{}

void WikipediaApplication::setup()
{
    minitel->defaultSetup();
    debugBegin();

    currentStep = Step::WifiLoading;
    minitel->displayNewPage("Loading...");

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

    case Step::SearchRequest:
        handleSearchRequestStep();
        break;

    case Step::ResultSelection:
        handleResultSelectionStep();
        break;

    case Step::DidYouMean:
        handleDidYouMeanStep();
        break;

    case Step::ArticleRequest:
        handleArticleRequestStep();
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
    minitel->displayString("Connection to Wi-fi...", 1, 3);
    minitel->moveCursorXY(1, 2);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        minitel->print(".");
    }
}

void WikipediaApplication::showPromptPage()
{
    currentStep = Step::UserInput;

    stringEnteredByUser = "";

    minitel->displayNewPage("3615 Wikipedia");
    minitel->displayString("What are you searching for?", 1, 3);
    minitel->moveCursorXY(1, 4);
    minitel->cursor();
}

void WikipediaApplication::showResultSelectionPage()
{
    minitel->displayNewPage("Choose an article:");

    const std::vector<WikipediaClient::SearchResult>& results = wikipediaClient.getSearchResults();
    searchResultsModel = std::make_shared<TableViewModel>(1, static_cast<int>(results.size()),
        [&results](int, int row) { return results[row].title; });
    searchResultsView = std::make_shared<TableView>(searchResultsModel.get(), StringList{ "Results" }, 0, 1);
    searchResultsView->setColumnWidth(0, 38);
    searchResultsView->setVerticalLinesEnabled(true);
    searchResultsView->draw(minitel);

    minitel->displayString("FLECHES to move, ENVOI to open", 1, 23);
    minitel->displayString("SOMMAIRE to search again", 1, 24);

    currentStep = Step::ResultSelection;
}

void WikipediaApplication::showDidYouMeanPage()
{
    minitel->displayNewPage("3615 Wikipedia");
    minitel->displayString("We found nothing for " + stringEnteredByUser, 1, 3);
    minitel->attributs(CARACTERE_ROUGE);
    minitel->displayString("Did you mean: " + wikipediaClient.getSuggestion() + " ?", 1, 5);
    minitel->attributs(CARACTERE_BLANC);
    minitel->displayString("ENVOI to search it", 1, 23);
    minitel->displayString("SOMMAIRE to search again", 1, 24);

    currentStep = Step::DidYouMean;
}

void WikipediaApplication::showResultPage()
{
    minitel->displayNewPage(wikipediaClient.getArticle().getTitle());

    if (resultPager.isEmpty())
    {
        minitel->displayString("We found nothing :(", 1, 3);
        wikipediaClient.getArticle().printToSerial();
    }
    else
    {
        minitel->attributs(CARACTERE_ROUGE);
        minitel->displayString(resultPager.getCurrentSectionTitle(), 1, 3);
        minitel->attributs(CARACTERE_BLANC);
        minitel->displayString("section " + String(resultPager.getCurrentSectionNumber()) + "/" +
            String(resultPager.getSectionCount()) + " - page " + String(resultPager.getCurrentPageNumber()), 1, 4);
        resultPager.displayPage(minitel, 6, maxPositionY);
    }

    currentStep = Step::DisplayResult;
}

void WikipediaApplication::showLoadingHint()
{
    minitel->displayString("Loading...", 1, maxPositionY);
}

void WikipediaApplication::handleUserInputStep()
{
    unsigned long key = minitel->updatePressedKey();
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
            currentStep = Step::SearchRequest;
        }
        break;

    default:
        String keyAsString = minitel->getString(key);
        minitel->print(keyAsString);
        stringEnteredByUser += keyAsString;
        break;
    }
}

void WikipediaApplication::handleSearchRequestStep()
{
    minitel->displayNewPage("3615 Wikipedia");
    minitel->displayString("Searching on Wikipedia...", 1, 3);

    if (WiFi.status() != WL_CONNECTED)
    {
        currentStep = Step::WifiLoading;
        return;
    }

    wikipediaClient.search(stringEnteredByUser);

    if (!wikipediaClient.getSearchResults().empty())
    {
        showResultSelectionPage();
    }
    else if (wikipediaClient.getSuggestion().length() > 0)
    {
        showDidYouMeanPage();
    }
    else
    {
        resultPager.reset(0);
        showResultPage();
    }
}

void WikipediaApplication::handleResultSelectionStep()
{
    unsigned long key = minitel->updatePressedKey();
    if (key == 0)
        return;

    switch (key)
    {
    case SOMMAIRE:
        showPromptPage();
        break;

    case ENVOI:
    {
        ModelIndex index = searchResultsView->getModelIndexUnderCursor();
        int row = index.getRow();
        const std::vector<WikipediaClient::SearchResult>& results = wikipediaClient.getSearchResults();
        if (row >= 0 && row < static_cast<int>(results.size()))
        {
            chosenPageId = results[row].pageId;
            currentStep = Step::ArticleRequest;
        }
        break;
    }

    case TOUCHE_FLECHE_HAUT:
        if (searchResultsView->moveVirtualCursor(CursorDirection::Up))
            searchResultsView->drawVirtualCursor(minitel);
        break;

    case TOUCHE_FLECHE_BAS:
        if (searchResultsView->moveVirtualCursor(CursorDirection::Down))
            searchResultsView->drawVirtualCursor(minitel);
        break;

    default:
        break;
    }
}

void WikipediaApplication::handleDidYouMeanStep()
{
    unsigned long key = minitel->updatePressedKey();
    if (key == 0)
        return;

    switch (key)
    {
    case ENVOI:
        stringEnteredByUser = wikipediaClient.getSuggestion();
        currentStep = Step::SearchRequest;
        break;

    case SOMMAIRE:
        showPromptPage();
        break;

    default:
        break;
    }
}

void WikipediaApplication::handleArticleRequestStep()
{
    minitel->displayNewPage("3615 Wikipedia");
    minitel->displayString("Loading article...", 1, 3);

    if (WiFi.status() != WL_CONNECTED)
    {
        currentStep = Step::WifiLoading;
        return;
    }

    wikipediaClient.fetchSectionList(chosenPageId);
    resultPager.reset(chosenPageId);
    showResultPage();
}

void WikipediaApplication::handleDisplayResultStep()
{
    unsigned long key = minitel->updatePressedKey();
    if (key == 0)
        return;

    switch (key)
    {
    case SOMMAIRE:
        showPromptPage();
        break;

    case SUITE:
        if (resultPager.nextWillFetch())
            showLoadingHint();
        if (resultPager.next())
            showResultPage();
        break;

    case RETOUR:
        if (resultPager.previousWillFetch())
            showLoadingHint();
        if (resultPager.previous())
            showResultPage();
        break;

    default:
        break;
    }
}
