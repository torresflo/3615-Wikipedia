#include "WikipediaResultPager.h"

#include "WikipediaClient.h"

WikipediaResultPager::WikipediaResultPager(WikipediaClient &client)
    :wikipediaClient(client)
{}

void WikipediaResultPager::reset(long pageId)
{
    this->pageId = pageId;
    loadSection(0);
}

bool WikipediaResultPager::isEmpty() const
{
    return wikipediaClient.getArticle().isEmpty();
}

void WikipediaResultPager::loadSection(int sectionIndex)
{
    pageSeparators.clear();
    pageSeparators.push_back(0);
    currentPageInSection = 0;
    currentPageIsLastInSection = true;
    currentSectionText = String();

    const std::vector<WikipediaArticle::Section>& sections = wikipediaClient.getArticle().getSections();
    if (sectionIndex < 0 || sectionIndex >= static_cast<int>(sections.size()))
    {
        currentSectionIndex = 0;
        return;
    }

    currentSectionIndex = sectionIndex;
    wikipediaClient.fetchSectionContent(pageId, sections.at(sectionIndex).index, currentSectionText);
}

bool WikipediaResultPager::nextWillFetch() const
{
    int sectionCount = getSectionCount();
    return currentPageIsLastInSection && (currentSectionIndex + 1 < sectionCount);
}

bool WikipediaResultPager::previousWillFetch() const
{
    return currentPageInSection == 0 && currentSectionIndex > 0;
}

bool WikipediaResultPager::next()
{
    if (!currentPageIsLastInSection)
    {
        currentPageInSection++;
        return true;
    }

    if (currentSectionIndex + 1 < getSectionCount())
    {
        loadSection(currentSectionIndex + 1);
        return true;
    }

    return false;
}

bool WikipediaResultPager::previous()
{
    if (currentPageInSection > 0)
    {
        currentPageInSection--;
        return true;
    }

    if (currentSectionIndex > 0)
    {
        loadSection(currentSectionIndex - 1);
        return true;
    }

    return false;
}

void WikipediaResultPager::displayPage(ExtendedMinitelPtr& minitel, unsigned int startLine, unsigned int endLine)
{
    unsigned int startChar = pageSeparators.at(currentPageInSection);
    unsigned int nextChar = minitel->displayVideotexLines(currentSectionText, startChar, startLine, endLine);

    currentPageIsLastInSection = (nextChar >= currentSectionText.length());

    if (!currentPageIsLastInSection && currentPageInSection + 1 >= static_cast<int>(pageSeparators.size()))
        pageSeparators.push_back(nextChar);
}

const String& WikipediaResultPager::getCurrentSectionTitle() const
{
    const std::vector<WikipediaArticle::Section>& sections = wikipediaClient.getArticle().getSections();
    return sections.at(currentSectionIndex).title;
}

int WikipediaResultPager::getCurrentSectionNumber() const
{
    return currentSectionIndex + 1;
}

int WikipediaResultPager::getSectionCount() const
{
    return static_cast<int>(wikipediaClient.getArticle().getSections().size());
}

int WikipediaResultPager::getCurrentPageNumber() const
{
    return currentPageInSection + 1;
}
