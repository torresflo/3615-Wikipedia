#ifndef WIKIPEDIARESULTPAGER_H
#define WIKIPEDIARESULTPAGER_H

#include <vector>

#include <MinitelToolkit.h>

class WikipediaResultPager
{
public:
    void reset(const String& newText);

    bool isEmpty() const;

    void displayPage(ExtendedMinitel& minitel, int pageIndex, unsigned int startLine, unsigned int endLine);

    int getCurrentPageIndex() const;

private:
    String text;
    std::vector<unsigned int> pageSeparators;
    int currentPageIndex = 0;
};

#endif
