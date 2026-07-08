#ifndef WIKIPEDIACLIENT_H
#define WIKIPEDIACLIENT_H

#include <Arduino.h>
#include <ArduinoJson.h>

#include "WikipediaArticle.h"

class WikipediaClient
{
public:
    bool search(const String& query);

    const WikipediaArticle& getArticle() const;

private:
    JsonObject getFirstPage(JsonDocument& document);
    void parseExtract(const String& extract);
    static bool parseSectionHeader(const String& line, int& levelOut, String& titleOut);

    WikipediaArticle article;

    static const char* serverPathPart1;
    static const char* serverPathPart2;
};

#endif
