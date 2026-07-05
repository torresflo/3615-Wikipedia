#include "WikipediaClient.h"

#include <WiFi.h>
#include <HTTPClient.h>

const char* WikipediaClient::serverPathPart1 = "https://en.wikipedia.org/w/api.php?action=query&format=json&titles=";
const char* WikipediaClient::serverPathPart2 = "&prop=extracts&exintro&explaintext";

JsonVariant WikipediaClient::findNestedKey(JsonObject obj, const char* key)
{
    JsonVariant foundObject = obj[key];
    if (!foundObject.isNull())
        return foundObject;

    for (JsonPair pair : obj)
    {
        JsonVariant nestedObject = findNestedKey(pair.value(), key);
        if (!nestedObject.isNull())
            return nestedObject;
    }

    return JsonVariant();
}

bool WikipediaClient::search(const String& query, String& extractOut)
{
    String serverPath = String(serverPathPart1) + query + String(serverPathPart2);

    HTTPClient client;
    client.begin(serverPath.c_str());

    int httpResponseCode = client.GET();

    if (httpResponseCode > 0)
    {
        String result = client.getString();

        JsonDocument jsonDocument;
        deserializeJson(jsonDocument, result);

        JsonVariant extractVariant = findNestedKey(jsonDocument.as<JsonObject>(), "extract");
        extractOut = extractVariant.isNull() ? String() : extractVariant.as<String>();
    }

    return httpResponseCode > 0;
}
