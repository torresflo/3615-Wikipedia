#include "ResultSelectionScreen.h"

#include <vector>

#include "WikipediaApplication.h"

ResultSelectionScreen::ResultSelectionScreen(WikipediaApplication& application)
    : Screen(application)
{}

void ResultSelectionScreen::setup()
{
    ExtendedMinitelPtr& minitel = application.getMinitel();
    WikipediaClient& client = application.getClient();

    minitel->displayNewPage("Choose an article:");

    const std::vector<WikipediaClient::SearchResult>& results = client.getSearchResults();
    searchResultsModel = std::make_shared<TableViewModel>(1, static_cast<int>(results.size()),
        [&results](int, int row) { return results[row].title; });
    searchResultsView = std::make_shared<TableView>(searchResultsModel.get(), StringList{ "Results" }, 0, 1);
    searchResultsView->setColumnWidth(0, 38);
    searchResultsView->setVerticalLinesEnabled(true);
    searchResultsView->draw(minitel);

    minitel->displayString("FLECHES to move, ENVOI to open", 1, 23);
    minitel->displayString("SOMMAIRE to search again", 1, 24);
}

bool ResultSelectionScreen::update(unsigned long key)
{
    if (key == 0)
        return true;

    ExtendedMinitelPtr& minitel = application.getMinitel();

    switch (key)
    {
    case SOMMAIRE:
        application.setNextScreenId(ScreenId::UserInput);
        return false;

    case ENVOI:
    {
        ModelIndex index = searchResultsView->getModelIndexUnderCursor();
        int row = index.getRow();
        const std::vector<WikipediaClient::SearchResult>& results = application.getClient().getSearchResults();
        if (row >= 0 && row < static_cast<int>(results.size()))
        {
            application.setChosenPageId(results[row].pageId);
            application.setNextScreenId(ScreenId::ArticleRequest);
            return false;
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

    return true;
}
