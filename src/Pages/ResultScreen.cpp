#include "ResultScreen.h"

#include "WikipediaApplication.h"

ResultScreen::ResultScreen(WikipediaApplication& application)
    : Screen(application)
{}

void ResultScreen::setup()
{
    showResultPage();
}

bool ResultScreen::update(unsigned long key)
{
    WikipediaResultPager& resultPager = application.getResultPager();

    switch (key)
    {
    case GUIDE:
        application.setNextScreenId(ScreenId::SectionList);
        return false;

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

    return true;
}

void ResultScreen::showResultPage()
{
    ExtendedMinitelPtr& minitel = application.getMinitel();
    WikipediaClient& client = application.getClient();
    WikipediaResultPager& resultPager = application.getResultPager();

    if (resultPager.isEmpty())
    {
        showEmptyResult();
    }
    else
    {
        minitel->displayNewPage(client.getArticle().getTitle());

        minitel->attributs(CARACTERE_ROUGE);
        minitel->displayString(resultPager.getCurrentSectionTitle(), 1, 3);
        minitel->attributs(CARACTERE_BLANC);
        minitel->displayString("section " + String(resultPager.getCurrentSectionNumber()) + "/" +
            String(resultPager.getSectionCount()) + " - page " + String(resultPager.getCurrentPageNumber()), 1, 4);
        resultPager.displayPage(minitel, 6, maxPositionY);
    }
}

void ResultScreen::showEmptyResult()
{
    ExtendedMinitelPtr& minitel = application.getMinitel();

    application.getClient().getArticle().printToSerial();

    minitel->displayNewPage("3615 Wikipedia");

    minitel->setTextSizeMode(TextSizeMode::DoubleSize);
    minitel->displayString("No results", 11, 8, Color::Red);
    minitel->setTextSizeMode(TextSizeMode::Normal);

    minitel->displayString("Nothing was found for:", 9, 12);

    String quotedQuery = "\"" + application.getQuery() + "\"";
    if (quotedQuery.length() > maxPositionX)
        quotedQuery = quotedQuery.substring(0, maxPositionX);
    int queryX = (maxPositionX - quotedQuery.length()) / 2 + 1;
    minitel->displayString(quotedQuery, queryX, 14, Color::Yellow);

    minitel->displayString("Press SOMMAIRE for a new search", 5, maxPositionY - 1, Color::Cyan);
}

void ResultScreen::showLoadingHint()
{
    application.getMinitel()->displayString("Loading...", minPositionX, maxPositionY);
}
