#ifndef WIKITEXT_H
#define WIKITEXT_H

#include <vector>

#include <Arduino.h>

class WikiTextHelper
{
public:
    static void toPlainText(String& text);

private:
    explicit WikiTextHelper(String& text);

    void run();

    // Appends a character to the rewritten text
    void emit(char c);
    void resyncEmitState();
    bool atLineStart() const;

    // Per-construct handlers. Each starts at readIndex and leaves it past what it consumed.
    void handleTag();
    void handleEntity();
    void handleWikilink();
    void handleExternalLink();
    void handleMagicWord();
    void handleQuotes();
    void handleHeadingLine();
    void handleListMarkers();
    void handleHorizontalRule();
    void skipNested(char open1, char open2, char close1, char close2);

    static bool isBlockTagName(const String& name);
    static char decodeEntity(const String& text, unsigned int& index);
    static bool isDropLinkTarget(const String& text, unsigned int start, unsigned int limit);

    String& text;
    const unsigned int length;
    unsigned int readIndex = 0;
    unsigned int writeIndex = 0;
    bool lastWasSpace = true; // trims leading spaces
    int newlineRun = 2;       // trims leading newlines
    std::vector<unsigned int> linkStack; // write positions of open, kept wikilinks
};

#endif
