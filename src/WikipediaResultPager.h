#ifndef WIKIPEDIARESULTPAGER_H
#define WIKIPEDIARESULTPAGER_H

#include <vector>

#include <MinitelToolkit.h>

class WikipediaClient;

class WikipediaResultPager
{
public:
    explicit WikipediaResultPager(WikipediaClient& client);

    void reset();

    bool isEmpty() const;
    void displayPage(ExtendedMinitel& minitel, int pageIndex, unsigned int startLine, unsigned int endLine);
    int getCurrentPageIndex() const;

private:
    std::vector<unsigned int> pageSeparators;
    int currentPageIndex = 0;

    WikipediaClient& wikipediaClient;
};

#endif
