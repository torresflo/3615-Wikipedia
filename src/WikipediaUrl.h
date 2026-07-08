#ifndef WIKIPEDIAURL_H
#define WIKIPEDIAURL_H

#include <Arduino.h>

class WikipediaUrl
{
public:
    static String search(const String& query);
    static String sections(long pageId);
    static String section(long pageId, int sectionIndex);

private:
    static String encode(const String& value);

    static const char* searchEndpoint;
    static const char* sectionsEndpoint;
    static const char* sectionEndpoint;
};

#endif
