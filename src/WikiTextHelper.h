#ifndef WIKITEXT_H
#define WIKITEXT_H

#include <Arduino.h>

class WikiTextHelper
{
public:
    static void toPlainText(String& text);

private:
    static bool isBlockTagName(const String& name);
    static char decodeEntity(const String& text, unsigned int& index);
    static bool isDropLinkTarget(const String& text, unsigned int start, unsigned int limit);
};

#endif
