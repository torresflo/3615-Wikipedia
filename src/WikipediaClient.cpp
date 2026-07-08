#include "WikipediaClient.h"

#include <utility>

#include <WiFi.h>
#include <HTTPClient.h>

const char* WikipediaClient::serverPathPart1 = "https://en.wikipedia.org/w/api.php?action=query&format=json&redirects=1&titles=";
const char* WikipediaClient::serverPathPart2 = "&prop=extracts|categories|description&explaintext&cllimit=30&clshow=!hidden";

const WikipediaArticle& WikipediaClient::getArticle() const
{
    return article;
}

JsonObject WikipediaClient::getFirstPage(JsonDocument& document)
{
    JsonObject pages = document["query"]["pages"].as<JsonObject>();
    for (JsonPair page : pages)
        return page.value().as<JsonObject>();

    return JsonObject();
}

bool WikipediaClient::parseSectionHeader(const String& line, int& levelOut, String& titleOut)
{
    unsigned int length = line.length();
    if (length < 4 || line[0] != '=' || line[length - 1] != '=')
        return false;

    unsigned int leading = 0;
    while (leading < length && line[leading] == '=')
        leading++;

    unsigned int trailing = 0;
    while (trailing < length && line[length - 1 - trailing] == '=')
        trailing++;

    if (leading < 2 || leading + trailing >= length)
        return false;

    String title = line.substring(leading, length - trailing);
    title.trim();
    if (title.length() == 0)
        return false;

    levelOut = static_cast<int>(leading);
    titleOut = std::move(title);
    return true;
}

void WikipediaClient::parseExtract(const String& extract)
{
    String pendingTitle;
    int pendingLevel = 0;
    bool inSection = false;
    String content;

    unsigned int lineStart = 0;
    unsigned int length = extract.length();
    while (true)
    {
        int newlineIndex = extract.indexOf('\n', lineStart);
        unsigned int lineEnd = (newlineIndex < 0) ? length : static_cast<unsigned int>(newlineIndex);
        String line = extract.substring(lineStart, lineEnd);

        String trimmed = line;
        trimmed.trim();

        int level = 0;
        String headerTitle;
        if (parseSectionHeader(trimmed, level, headerTitle))
        {
            content.trim();
            if (inSection)
                article.addSection(std::move(pendingTitle), pendingLevel, std::move(content));
            else
                article.setSummary(std::move(content));

            pendingTitle = std::move(headerTitle);
            pendingLevel = level;
            inSection = true;
            content = String();
        }
        else
        {
            if (content.length() > 0)
                content += '\n';
            content += line;
        }

        if (newlineIndex < 0)
            break;
        lineStart = lineEnd + 1;
    }

    content.trim();
    if (inSection)
        article.addSection(std::move(pendingTitle), pendingLevel, std::move(content));
    else
        article.setSummary(std::move(content));
}

bool WikipediaClient::search(const String& query)
{
    article.clear();

    String serverPath = String(serverPathPart1) + query + String(serverPathPart2);

    HTTPClient client;
    client.begin(serverPath.c_str());

    int httpResponseCode = client.GET();

    bool success = false;
    if (httpResponseCode > 0)
    {
        JsonDocument jsonDocument;
        DeserializationError error = deserializeJson(jsonDocument, client.getString());
        
        if (!error)
        {
            JsonObject page = getFirstPage(jsonDocument);
            if (!page.isNull())
            {
                JsonVariant titleVariant = page["title"];
                if (!titleVariant.isNull())
                    article.setTitle(titleVariant.as<String>());

                JsonVariant descriptionVariant = page["description"];
                if (!descriptionVariant.isNull())
                    article.setDescription(descriptionVariant.as<String>());

                JsonVariant extractVariant = page["extract"];
                if (!extractVariant.isNull())
                    parseExtract(extractVariant.as<String>());

                for (JsonObject category : page["categories"].as<JsonArray>())
                {
                    String name = category["title"].as<String>();
                    if (name.startsWith("Category:"))
                        name = name.substring(9);
                    article.addCategory(std::move(name));
                }

                success = true;
            }
        }
    }

    client.end();
    return success;
}
