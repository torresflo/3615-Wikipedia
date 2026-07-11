#include "WikipediaClient.h"

#include <utility>

#include <WiFi.h>
#include <HTTPClient.h>

#include <MinitelToolkit.h>

#include "WikiTextHelper.h"
#include "WikipediaUrl.h"

const unsigned int WikipediaClient::maxSectionLength = 12000;

const std::vector<WikipediaClient::SearchResult>& WikipediaClient::getSearchResults() const
{
    return searchResults;
}

const String& WikipediaClient::getSuggestion() const
{
    return suggestion;
}

const WikipediaArticle& WikipediaClient::getArticle() const
{
    return article;
}

bool WikipediaClient::isNoiseSection(const String& title)
{
    return title == "References" || title == "External links" || title == "See also" ||
           title == "Notes" || title == "Bibliography" || title == "Further reading" ||
           title == "Citations" || title == "Sources" || title == "Notes and references";
}

bool WikipediaClient::search(const String& query)
{
    searchResults.clear();
    suggestion = String();
    article.clear();

    String serverPath = WikipediaUrl::search(query);
    debugPrintf("Processing search of '%s' with path: %s\n", query, serverPath.c_str());

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
            for (JsonObject result : jsonDocument["query"]["search"].as<JsonArray>())
            {
                JsonVariant titleVariant = result["title"];
                JsonVariant pageIdVariant = result["pageid"];
                if (!titleVariant.isNull() && !pageIdVariant.isNull())
                    searchResults.push_back(SearchResult{titleVariant.as<String>(), pageIdVariant.as<long>()});
            }

            JsonVariant suggestionVariant = jsonDocument["query"]["searchinfo"]["suggestion"];
            if (!suggestionVariant.isNull())
                suggestion = suggestionVariant.as<String>();

            success = true;
        }
        else
            debugPrintf("Error during JSON deserialization: %s\n", error.c_str());
    }
    else
        debugPrintf("Error during HTTP request: %d\n", httpResponseCode);

    client.end();
    return success;
}

bool WikipediaClient::fetchSectionList(long pageId)
{
    article.clear();

    String serverPath = WikipediaUrl::sections(pageId);
    debugPrintf("Retrieving section list of page '%ld' with path: %s\n", pageId, serverPath.c_str());

    HTTPClient client;
    client.begin(serverPath.c_str());

    int httpResponseCode = client.GET();

    bool success = false;
    if (httpResponseCode > 0)
    {
        JsonDocument filter;
        filter["parse"]["title"] = true;
        filter["parse"]["tocdata"]["sections"][0]["line"] = true;
        filter["parse"]["tocdata"]["sections"][0]["index"] = true;
        filter["parse"]["tocdata"]["sections"][0]["tocLevel"] = true;

        JsonDocument jsonDocument;
        DeserializationError error = deserializeJson(jsonDocument, client.getString(), DeserializationOption::Filter(filter));

        if (!error)
        {
            JsonObject parse = jsonDocument["parse"].as<JsonObject>();
            if (!parse.isNull())
            {
                JsonVariant titleVariant = parse["title"];
                if (!titleVariant.isNull())
                    article.setTitle(titleVariant.as<String>());

                // The lead/intro is section 0 and is not listed by the API.
                article.addSection(String("Introduction"), 1, 0);

                for (JsonObject section : parse["tocdata"]["sections"].as<JsonArray>())
                {
                    // "index" may be non-numeric for transcluded sections (e.g. "T-1"), which as<int>() yields as 0; skip those.
                    int index = section["index"].as<int>();
                    if (index <= 0)
                        continue;

                    String line = section["line"].as<String>();
                    WikiTextHelper::toPlainText(line);
                    if (isNoiseSection(line))
                        continue;

                    int level = section["tocLevel"] | 1;
                    article.addSection(std::move(line), level, index);
                }

                success = !article.isEmpty();
            }
        }
        else
            debugPrintf("Error during JSON deserialization: %s\n", error.c_str());
    }
    else
        debugPrintf("Error during HTTP request: %d\n", httpResponseCode);

    client.end();
    return success;
}

bool WikipediaClient::fetchSectionContent(long pageId, int sectionIndex, String& outText)
{
    outText = String();

    String serverPath = WikipediaUrl::section(pageId, sectionIndex);
    debugPrintf("Retrieving section '%d' of page '%ld' with path: %s\n", sectionIndex, pageId, serverPath.c_str());

    HTTPClient client;
    client.begin(serverPath.c_str());

    int httpResponseCode = client.GET();

    bool success = false;
    if (httpResponseCode > 0)
    {
        JsonDocument filter;
        filter["parse"]["wikitext"] = true;

        JsonDocument jsonDocument;
        DeserializationError error = deserializeJson(jsonDocument, client.getString(), DeserializationOption::Filter(filter));

        if (!error)
        {
            JsonVariant textVariant = jsonDocument["parse"]["wikitext"];
            if (!textVariant.isNull())
            {
                outText = textVariant.as<String>();
                WikiTextHelper::stripSubsections(outText);
                WikiTextHelper::toPlainText(outText);
                if (outText.length() > maxSectionLength)
                    outText.remove(maxSectionLength);
                success = true;
            }
        }
        else
            debugPrintf("Error during JSON deserialization: %s\n", error.c_str());
    }
    else
        debugPrintf("Error during HTTP request: %d\n", httpResponseCode);

    client.end();
    return success;
}
