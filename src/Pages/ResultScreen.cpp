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
    if (key == 0)
        return true;

    WikipediaResultPager& resultPager = application.getResultPager();

    switch (key)
    {
    case SOMMAIRE:
        application.setNextScreenId(ScreenId::UserInput);
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

    minitel->displayNewPage(client.getArticle().getTitle());

    if (resultPager.isEmpty())
    {
        minitel->displayString("We found nothing :(", 1, 3);
        client.getArticle().printToSerial();
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
}

void ResultScreen::showLoadingHint()
{
    application.getMinitel()->displayString("Loading...", 1, maxPositionY);
}
