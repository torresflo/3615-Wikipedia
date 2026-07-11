#ifndef WIKIPEDIARESULTPAGER_H
#define WIKIPEDIARESULTPAGER_H

#include <vector>

#include <MinitelToolkit.h>

class WikipediaClient;

class WikipediaResultPager
{
public:
    explicit WikipediaResultPager(WikipediaClient& client);

    void reset(long pageId);

    bool isEmpty() const;

    // Navigation. Returns true if a redraw is needed
    bool next();
    bool previous();
    bool goToSection(int sectionIndex);

    bool nextWillFetch() const;
    bool previousWillFetch() const;

    void displayPage(ExtendedMinitelPtr& minitel, unsigned int startLine, unsigned int endLine);

    const String& getCurrentSectionTitle() const;
    int getCurrentSectionNumber() const; // 1-based
    int getSectionCount() const;
    int getCurrentPageNumber() const;    // 1-based, within the current section

private:
    void loadSection(int sectionIndex);
    void loadSectionSkippingEmpty(int startIndex, int direction);

    WikipediaClient& wikipediaClient;

    long pageId = 0;
    int currentSectionIndex = 0;
    String currentSectionText;
    std::vector<unsigned int> pageSeparators;
    int currentPageInSection = 0;
    bool currentPageIsLastInSection = true;
};

#endif
