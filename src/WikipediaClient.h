#ifndef WIKIPEDIACLIENT_H
#define WIKIPEDIACLIENT_H

#include <vector>

#include <Arduino.h>
#include <ArduinoJson.h>

#include "WikipediaArticle.h"

class WikipediaClient
{
public:
    struct SearchResult
    {
        String title;
        long pageId;
    };

    bool search(const String& query);
    bool fetchSectionList(long pageId);
    bool fetchSectionContent(long pageId, int sectionIndex, String& outText);

    const std::vector<SearchResult>& getSearchResults() const;
    const String& getSuggestion() const;
    const WikipediaArticle& getArticle() const;

private:
    static bool isNoiseSection(const String& title);

    static const unsigned int maxSectionLength;

    std::vector<SearchResult> searchResults;
    String suggestion;
    WikipediaArticle article;
};

#endif
