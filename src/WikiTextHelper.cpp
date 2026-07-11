#include "WikiTextHelper.h"

bool WikiTextHelper::isBlockTagName(const String& name)
{
    return name == "p" || name == "br" || name == "div" || name == "li" ||
           name == "tr" || name == "ul" || name == "ol" ||
           name == "h1" || name == "h2" || name == "h3" ||
           name == "h4" || name == "h5" || name == "h6" ||
           name == "blockquote" || name == "section" ||
           name == "dd" || name == "dt" || name == "caption" || name == "figcaption";
}

char WikiTextHelper::decodeEntity(const String& text, unsigned int& index)
{
    unsigned int length = text.length();
    unsigned int limit = index + 10;
    if (limit > length)
        limit = length;

    unsigned int semicolon = index;
    while (semicolon < limit && text[semicolon] != ';')
        semicolon++;

    if (semicolon >= limit || text[semicolon] != ';')
        return 0;

    String entity = text.substring(index, semicolon);
    char result = 0;

    if (entity == "amp")
        result = '&';
    else if (entity == "lt")
        result = '<';
    else if (entity == "gt")
        result = '>';
    else if (entity == "quot")
        result = '"';
    else if (entity == "apos")
        result = '\'';
    else if (entity == "nbsp")
        result = ' ';
    else if (entity == "mdash" || entity == "ndash")
        result = '-';
    else if (entity.length() > 1 && entity[0] == '#')
    {
        long code;
        if (entity[1] == 'x' || entity[1] == 'X')
            code = strtol(entity.substring(2).c_str(), nullptr, 16);
        else
            code = entity.substring(1).toInt();

        if (code == 160)
            result = ' ';
        else if (code >= 32 && code < 127)
            result = static_cast<char>(code);
        else
            result = ' '; // non-ASCII char: best-effort fallback
    }

    if (result == 0)
        return 0;

    index = semicolon + 1;
    return result;
}

bool WikiTextHelper::isDropLinkTarget(const String& text, unsigned int start, unsigned int limit)
{
    unsigned int i = start;
    while (i < limit && (text[i] == ' ' || text[i] == ':'))
        i++;

    String ns;
    while (i < limit)
    {
        char ch = text[i];
        if (ch == '|' || ch == ']' || ch == '\n' || ch == ':')
            break;
        if (ch >= 'A' && ch <= 'Z')
            ch = ch - 'A' + 'a';
        if (ch >= 'a' && ch <= 'z')
        {
            ns += ch;
            i++;
        }
        else
            break;
    }

    while (i < limit && text[i] == ' ')
        i++;
    if (i >= limit || text[i] != ':')
        return false;

    return ns == "file" || ns == "image" || ns == "category" || ns == "media";
}

int WikiTextHelper::headingLevel(const String& text, unsigned int lineStart, unsigned int lineEnd)
{
    unsigned int i = lineStart;
    while (i < lineEnd && (text[i] == ' ' || text[i] == '\t'))
        i++;

    unsigned int openStart = i;
    while (i < lineEnd && text[i] == '=')
        i++;
    unsigned int openCount = i - openStart;
    if (openCount < 1)
        return 0;

    unsigned int end = lineEnd;
    while (end > i && (text[end - 1] == ' ' || text[end - 1] == '\t' || text[end - 1] == '\r'))
        end--;

    unsigned int close = end;
    while (close > i && text[close - 1] == '=')
        close--;
    unsigned int closeCount = end - close;

    // A heading needs equal-sign runs on both sides and a title in between.
    if (closeCount < 1 || close <= i)
        return 0;

    return static_cast<int>(openCount < closeCount ? openCount : closeCount);
}

void WikiTextHelper::stripSubsections(String& text)
{
    unsigned int length = text.length();
    unsigned int lineStart = 0;
    int sectionLevel = 0;
    bool sectionHeadingSeen = false;

    while (lineStart < length)
    {
        unsigned int lineEnd = lineStart;
        while (lineEnd < length && text[lineEnd] != '\n')
            lineEnd++;

        int level = headingLevel(text, lineStart, lineEnd);
        if (level > 0)
        {
            if (!sectionHeadingSeen)
            {
                sectionHeadingSeen = true;
                sectionLevel = level;
            }
            else if (level > sectionLevel)
            {
                // First subsection heading: drop it and everything after.
                text.remove(lineStart);
                return;
            }
        }

        lineStart = lineEnd + 1;
    }
}

void WikiTextHelper::toPlainText(String& text)
{
    WikiTextHelper parser(text);
    parser.run();
}

WikiTextHelper::WikiTextHelper(String& text)
    : text(text), length(text.length())
{}

bool WikiTextHelper::atLineStart() const
{
    return newlineRun > 0;
}

void WikiTextHelper::emit(char c)
{
    if (c == '\n')
    {
        if (newlineRun >= 2)
            return;
        text.setCharAt(writeIndex++, '\n');
        newlineRun++;
        lastWasSpace = false;
    }
    else if (c == ' ' || c == '\t' || c == '\r')
    {
        if (newlineRun > 0 || lastWasSpace)
            return;
        text.setCharAt(writeIndex++, ' ');
        lastWasSpace = true;
    }
    else
    {
        text.setCharAt(writeIndex++, c);
        lastWasSpace = false;
        newlineRun = 0;
    }
}

void WikiTextHelper::resyncEmitState()
{
    if (writeIndex == 0)
    {
        lastWasSpace = true;
        newlineRun = 2;
        return;
    }

    char previous = text[writeIndex - 1];
    if (previous == '\n')
    {
        lastWasSpace = false;
        newlineRun = 1;
    }
    else if (previous == ' ')
    {
        lastWasSpace = true;
        newlineRun = 0;
    }
    else
    {
        lastWasSpace = false;
        newlineRun = 0;
    }
}

void WikiTextHelper::skipNested(char open1, char open2, char close1, char close2)
{
    int depth = 0;
    unsigned int i = readIndex;
    while (i + 1 < length)
    {
        if (text[i] == open1 && text[i + 1] == open2)
        {
            depth++;
            i += 2;
        }
        else if (text[i] == close1 && text[i + 1] == close2)
        {
            depth--;
            i += 2;
            if (depth == 0)
                break;
        }
        else
            i++;
    }
    readIndex = (depth != 0) ? length : i;
}

void WikiTextHelper::handleTag()
{
    // HTML comment
    if (readIndex + 3 < length && text[readIndex + 1] == '!' &&
        text[readIndex + 2] == '-' && text[readIndex + 3] == '-')
    {
        int end = text.indexOf("-->", readIndex + 4);
        readIndex = (end < 0) ? length : static_cast<unsigned int>(end) + 3;
        return;
    }

    unsigned int nameStart = readIndex + 1;
    bool closing = (nameStart < length && text[nameStart] == '/');
    if (closing)
        nameStart++;

    String name;
    unsigned int p = nameStart;
    while (p < length)
    {
        char t = text[p];
        if (t >= 'A' && t <= 'Z')
            t = t - 'A' + 'a';
        if ((t >= 'a' && t <= 'z') || (t >= '0' && t <= '9'))
        {
            name += t;
            p++;
        }
        else
            break;
    }

    int gt = text.indexOf('>', p);
    unsigned int tagEnd = (gt < 0) ? length : static_cast<unsigned int>(gt) + 1;
    bool selfClosing = (gt > 0 && text[gt - 1] == '/');

    // Drop these elements together with their content.
    if (!closing && !selfClosing &&
        (name == "style" || name == "table" || name == "script" || name == "sup" || name == "ref"))
    {
        String closeTag = "</" + name + ">";
        int end = text.indexOf(closeTag, tagEnd);
        readIndex = (end < 0) ? length : static_cast<unsigned int>(end) + closeTag.length();
        return;
    }

    if (isBlockTagName(name))
        emit('\n');

    readIndex = tagEnd;
}

void WikiTextHelper::handleEntity()
{
    unsigned int index = readIndex + 1;
    char decoded = decodeEntity(text, index);
    if (decoded != 0)
    {
        emit(decoded);
        readIndex = index;
        return;
    }
    emit('&');
    readIndex++;
}

void WikiTextHelper::handleWikilink()
{
    if (isDropLinkTarget(text, readIndex + 2, length))
    {
        // File/Image/Category/Media link: drop entirely, nesting-aware.
        skipNested('[', '[', ']', ']');
        return;
    }

    // Normal wikilink: keep emitting inner text; a later '|' discards the target.
    linkStack.push_back(writeIndex);
    readIndex += 2;
}

void WikiTextHelper::handleExternalLink()
{
    // External link [url label] -> keep only the label; bare [url] is dropped.
    bool isScheme = text.startsWith("http", readIndex + 1) ||
                    text.startsWith("//", readIndex + 1);
    if (isScheme)
    {
        int close = text.indexOf(']', readIndex + 1);
        unsigned int end = (close < 0) ? length : static_cast<unsigned int>(close);
        unsigned int s = readIndex + 1;
        while (s < end && text[s] != ' ' && text[s] != '\t')
            s++;
        for (unsigned int k = s + 1; k < end; k++)
            emit(text[k]);
        readIndex = (close < 0) ? length : end + 1;
        return;
    }
    emit('[');
    readIndex++;
}

void WikiTextHelper::handleMagicWord()
{
    // Behaviour switch magic word, e.g. __TOC__ / __NOTOC__.
    int end = text.indexOf("__", readIndex + 2);
    if (end > static_cast<int>(readIndex + 2))
    {
        bool allUpper = true;
        for (int k = readIndex + 2; k < end; k++)
        {
            char ch = text[k];
            if (!(ch >= 'A' && ch <= 'Z'))
            {
                allUpper = false;
                break;
            }
        }
        if (allUpper && (end - static_cast<int>(readIndex + 2)) <= 20)
        {
            readIndex = static_cast<unsigned int>(end) + 2;
            return;
        }
    }
    emit('_');
    readIndex++;
}

void WikiTextHelper::handleQuotes()
{
    // Runs of 2+ apostrophes are bold/italic markers; a lone one is a real apostrophe.
    unsigned int q = readIndex;
    while (q < length && text[q] == '\'')
        q++;
    if (q - readIndex >= 2)
    {
        readIndex = q;
        return;
    }
    emit('\'');
    readIndex++;
}

void WikiTextHelper::handleHeadingLine()
{
    // Heading line: == Title == -> drop the whole line
    while (readIndex < length && text[readIndex] != '\n')
        readIndex++;
}

void WikiTextHelper::handleListMarkers()
{
    // List / indentation markers at line start.
    while (readIndex < length &&
           (text[readIndex] == '*' || text[readIndex] == '#' ||
            text[readIndex] == ':' || text[readIndex] == ';'))
        readIndex++;
    while (readIndex < length && (text[readIndex] == ' ' || text[readIndex] == '\t'))
        readIndex++;
}

void WikiTextHelper::handleHorizontalRule()
{
    // Horizontal rule: a line of 4+ dashes.
    unsigned int d = readIndex;
    while (d < length && text[d] == '-')
        d++;
    if (d - readIndex >= 4)
    {
        while (d < length && text[d] != '\n')
            d++;
        readIndex = d;
        return;
    }
    emit('-');
    readIndex++;
}

void WikiTextHelper::run()
{
    while (readIndex < length)
    {
        char c = text[readIndex];

        if (c == '<')
            handleTag();
        else if (c == '&')
            handleEntity();
        else if (c == '{' && readIndex + 1 < length && text[readIndex + 1] == '{')
            skipNested('{', '{', '}', '}'); // template
        else if (c == '{' && readIndex + 1 < length && text[readIndex + 1] == '|')
            skipNested('{', '|', '|', '}'); // table
        else if (c == '[' && readIndex + 1 < length && text[readIndex + 1] == '[')
            handleWikilink();
        else if (c == ']' && readIndex + 1 < length && text[readIndex + 1] == ']' && !linkStack.empty())
        {
            linkStack.pop_back();
            readIndex += 2;
        }
        else if (c == '|' && !linkStack.empty())
        {
            // Label separator: discard everything emitted for the target so far.
            writeIndex = linkStack.back();
            resyncEmitState();
            readIndex++;
        }
        else if (c == '[')
            handleExternalLink();
        else if (c == '_' && readIndex + 1 < length && text[readIndex + 1] == '_')
            handleMagicWord();
        else if (c == '\'')
            handleQuotes();
        else if (atLineStart() && c == '=')
            handleHeadingLine();
        else if (atLineStart() && (c == '*' || c == '#' || c == ':' || c == ';'))
            handleListMarkers();
        else if (atLineStart() && c == '-')
            handleHorizontalRule();
        else
        {
            emit(c);
            readIndex++;
        }
    }

    // Trim trailing whitespace.
    while (writeIndex > 0)
    {
        char c = text[writeIndex - 1];
        if (c == ' ' || c == '\n' || c == '\t' || c == '\r')
            writeIndex--;
        else
            break;
    }

    text.remove(writeIndex);
}
