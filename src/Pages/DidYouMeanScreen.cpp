#include "DidYouMeanScreen.h"

#include "WikipediaApplication.h"

DidYouMeanScreen::DidYouMeanScreen(WikipediaApplication& application)
    : Screen(application)
{}

void DidYouMeanScreen::setup()
{
    ExtendedMinitelPtr& minitel = application.getMinitel();
    WikipediaClient& client = application.getClient();

    minitel->displayNewPage("3615 Wikipedia");
    minitel->displayString("We found nothing for " + application.getQuery(), 1, 3);
    minitel->attributs(CARACTERE_ROUGE);
    minitel->displayString("Did you mean: " + client.getSuggestion() + " ?", 1, 5);
    minitel->attributs(CARACTERE_BLANC);
    minitel->displayString("ENVOI to search it", 1, 23);
    minitel->displayString("SOMMAIRE to search again", 1, 24);
}

bool DidYouMeanScreen::update(unsigned long key)
{
    if (key == 0)
        return true;

    switch (key)
    {
    case ENVOI:
        application.setQuery(application.getClient().getSuggestion());
        application.setNextScreenId(ScreenId::SearchRequest);
        return false;

    default:
        break;
    }

    return true;
}
