#include "WikipediaResultPager.h"

#include "WikipediaClient.h"

void WikipediaResultPager::reset()
{
    pageSeparators.clear();
    pageSeparators.push_back(0);
    currentPageIndex = 0;
}

WikipediaResultPager::WikipediaResultPager(WikipediaClient &client)
    :wikipediaClient(client)
{}

bool WikipediaResultPager::isEmpty() const
{
    return wikipediaClient.getArticle().isEmpty();
}

void WikipediaResultPager::displayPage(ExtendedMinitel& minitel, int pageIndex, unsigned int startLine, unsigned int endLine)
{
    const String& text = wikipediaClient.getArticle().getSummary();

    if (pageIndex < 0)
        pageIndex = 0;
    if (pageIndex >= static_cast<int>(pageSeparators.size()))
        pageIndex = static_cast<int>(pageSeparators.size()) - 1;

    unsigned int charIndex = pageSeparators.at(pageIndex);
    unsigned int nextCharIndex = minitel.displayVideotexLines(text, charIndex, startLine, endLine);

    if (nextCharIndex < text.length() && pageIndex + 1 >= static_cast<int>(pageSeparators.size()))
    {
        pageSeparators.push_back(nextCharIndex);
    }

    currentPageIndex = pageIndex;
}

int WikipediaResultPager::getCurrentPageIndex() const
{
    return currentPageIndex;
}
