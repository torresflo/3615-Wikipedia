#ifndef WIKIPEDIACLIENT_H
#define WIKIPEDIACLIENT_H

#include <Arduino.h>
#include <ArduinoJson.h>

class WikipediaClient
{
public:
    bool search(const String& query, String& extractOut);

private:
    JsonVariant findNestedKey(JsonObject obj, const char* key);

    static const char* serverPathPart1;
    static const char* serverPathPart2;
};

#endif
