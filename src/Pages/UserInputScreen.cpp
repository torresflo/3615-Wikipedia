#include "UserInputScreen.h"

#include "WikipediaApplication.h"

UserInputScreen::UserInputScreen(WikipediaApplication& application)
    : Screen(application)
{}

void UserInputScreen::setup()
{
    stringEnteredByUser = "";

    ExtendedMinitelPtr& minitel = application.getMinitel();
    minitel->displayNewPage("3615 Wikipedia");
    minitel->displayString("What are you searching for?", 1, 3);
    minitel->moveCursorXY(1, 4);
    minitel->cursor();
}

bool UserInputScreen::update(unsigned long key)
{
    if (key == 0)
        return true;

    switch (key)
    {
    case TOUCHE_FLECHE_GAUCHE:
        break;

    case TOUCHE_FLECHE_DROITE:
        break;

    case ENVOI:
        if (stringEnteredByUser.length() > 0)
        {
            application.setQuery(stringEnteredByUser);
            application.setNextScreenId(ScreenId::SearchRequest);
            application.getMinitel()->noCursor();
            return false;
        }
        break;

    default:
        {
            ExtendedMinitelPtr& minitel = application.getMinitel();
            String keyAsString = minitel->getString(key);
            minitel->print(keyAsString);
            stringEnteredByUser += keyAsString;
            break;
        }
    }

    return true;
}
