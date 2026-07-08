#include "WikipediaUrl.h"

const char* WikipediaUrl::searchEndpoint = "https://en.wikipedia.org/w/api.php?action=query&format=json&list=search&srinfo=suggestion&srprop=&srlimit=15&srsearch=";
const char* WikipediaUrl::sectionsEndpoint = "https://en.wikipedia.org/w/api.php?action=parse&format=json&formatversion=2&prop=tocdata&pageid=";
const char* WikipediaUrl::sectionEndpoint = "https://en.wikipedia.org/w/api.php?action=parse&format=json&formatversion=2&prop=wikitext";

String WikipediaUrl::search(const String& query)
{
    return String(searchEndpoint) + encode(query);
}

String WikipediaUrl::sections(long pageId)
{
    return String(sectionsEndpoint) + String(pageId);
}

String WikipediaUrl::section(long pageId, int sectionIndex)
{
    return String(sectionEndpoint) + "&section=" + String(sectionIndex) + "&pageid=" + String(pageId);
}

String WikipediaUrl::encode(const String& value)
{
    String encoded;
    encoded.reserve(value.length() * 3);

    for (unsigned int i = 0; i < value.length(); i++)
    {
        char c = value[i];
        bool isUnreserved = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
                            (c >= '0' && c <= '9') ||
                            c == '-' || c == '_' || c == '.' || c == '~';

        if (isUnreserved)
        {
            encoded += c;
        }
        else
        {
            static const char* hexDigits = "0123456789ABCDEF";
            unsigned char byte = static_cast<unsigned char>(c);
            encoded += '%';
            encoded += hexDigits[byte >> 4];
            encoded += hexDigits[byte & 0x0F];
        }
    }

    return encoded;
}
