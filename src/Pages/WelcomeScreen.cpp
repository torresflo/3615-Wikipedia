#include "WelcomeScreen.h"

#include "WikipediaApplication.h"

WelcomeScreen::WelcomeScreen(WikipediaApplication& application)
    : Screen(application)
{}

void WelcomeScreen::setup()
{
    ExtendedMinitelPtr& minitel = application.getMinitel();
    minitel->displayNewPage("3615 Wikipedia");

    minitel->setTextSizeMode(TextSizeMode::DoubleSize);
    minitel->displayString("3615 Wikipedia", 7, 7);
    minitel->setTextSizeMode(TextSizeMode::Normal);

    minitel->displayString("Type a subject then press ENVOI", 5, maxPositionY - 1, Color::Cyan);

    minitel->textLineEdit("What are you searching for?", 7, 11, 25);
    minitel->cursor();
}

bool WelcomeScreen::update(unsigned long key)
{
    if (key == 0)
        return true;

    ExtendedMinitelPtr& minitel = application.getMinitel();

    String enteredText;
    if (!minitel->updateReadUserString(key, enteredText))
        return true;

    if (enteredText.length() == 0)
        return true;

    application.setQuery(enteredText);
    application.setNextScreenId(ScreenId::SearchRequest);
    minitel->noCursor();
    return false;
}
