#include <vector>

#include <WiFi.h>
#include <HTTPClient.h>

#include <ArduinoJson.h>
#include <Minitel1B_Hard.h>

//////// Minitel configuration ////////

#define MINITEL_PORT Serial2 // for Minitel-ESP32 devboard
#define MINITEL_VIDEOTEXT_LINES 24
#define MINITEL_VIDEOTEXT_COLUMNS 40

Minitel minitel(MINITEL_PORT);

//////// WiFi credentials ///////

const char* ssid     = "<YOUR SSID HERE>";
const char* password = "<YOUR PASSWORD HERE>";

//////// Wikipedia result ///////

JsonDocument jsonDocument;
JsonDocument jsonFilter;

String wikipediaPage;
std::vector<unsigned int> pageSeparators;
int currentPageSeparatorsIndex = 0;

//////////////////////////////////////////

#define STEP_WIFI_LOADING 0
#define STEP_USER_INPUT 1
#define STEP_HTTP_REQUEST 2
#define STEP_DISPLAY_RESULT 3

String serverPathPart1 = "https://en.wikipedia.org/w/api.php?action=query&format=json&titles=";
String serverPathPart2 = "&prop=extracts&exintro&explaintext";

int currentStep = STEP_WIFI_LOADING;
String stringEnteredByUser = "";

JsonVariant findNestedKey(JsonObject obj, const char *key)
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

unsigned int displayVideotexLines(String &text, unsigned int startCharacter, unsigned int startLine, unsigned int endLine)
{
    if (startCharacter > text.length() - 1)
        return text.length();

    unsigned int currentStartChar = startCharacter;
    unsigned int currentEndChar = currentStartChar;

    for (unsigned int i = startLine; i <= endLine; ++i)
    {
        minitel.moveCursorXY(0, i);
        for (unsigned int j = 0; j <= MINITEL_VIDEOTEXT_COLUMNS; ++j)
        {
            unsigned int charIndex = currentStartChar + j;
            if (charIndex >= text.length())
            {
                minitel.print(text.substring(currentStartChar));
                return text.length();
            }

            if (text.charAt(charIndex) == ' ')
            {
                currentEndChar = charIndex;
            }
        }

        minitel.print(text.substring(currentStartChar, currentEndChar));
        currentStartChar = currentEndChar + 1;
    }
    return currentStartChar;
}

void displayNewPage(String title)
{
    minitel.newScreen();
    minitel.textMode();
    minitel.noCursor();
    minitel.newXY(1, 0);
    minitel.attributs(CARACTERE_ROUGE);
    minitel.print(title);
    minitel.cancel();
    minitel.moveCursorDown(1);

    minitel.attributs(CARACTERE_ROUGE);
    for (int i = 1; i <= 40; i++)
    {
        minitel.writeByte(0x7E);
    }

    minitel.moveCursorReturn(0);
    minitel.attributs(CARACTERE_BLANC);
}

void displayPromptPage()
{
    currentStep = STEP_USER_INPUT;

    stringEnteredByUser = "";

    displayNewPage("3615 Wikipedia");
    minitel.println("What are you searching for?");
    minitel.cursor();
}

void displayWikipediaPage(int pageSeparatorsIndex)
{
    displayNewPage("3615 Wikipedia");
    minitel.attributs(CARACTERE_ROUGE);
    minitel.println("Result for \"" + stringEnteredByUser + "\":");
    minitel.attributs(CARACTERE_BLANC);

    if (pageSeparatorsIndex < 0)
        pageSeparatorsIndex = 0;
    if (pageSeparatorsIndex >= pageSeparators.size())
        pageSeparatorsIndex = pageSeparators.size() - 1;

    if (wikipediaPage.length() == 0)
    {
        minitel.println("We found nothing :(");
    }
    else
    {
        unsigned int charIndex = pageSeparators.at(pageSeparatorsIndex);
        unsigned int separatorIndex = displayVideotexLines(wikipediaPage, charIndex, 3, MINITEL_VIDEOTEXT_LINES);
        if (separatorIndex < wikipediaPage.length() && pageSeparatorsIndex + 1 >= pageSeparators.size())
        {
            pageSeparators.push_back(separatorIndex);
        }
        currentPageSeparatorsIndex = pageSeparatorsIndex;
    }

    currentStep = STEP_DISPLAY_RESULT;
}

void setup()
{
    minitel.changeSpeed(minitel.searchSpeed());
    minitel.echo(false);
    minitel.smallMode();
    minitel.extendedKeyboard();

    currentStep = STEP_WIFI_LOADING;
    displayNewPage("Loading...");

    minitel.print("Connection to Wi-fi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        minitel.print(".");
    }
    minitel.println("DONE");

    displayPromptPage();
}

void loop()
{
    if (currentStep == STEP_USER_INPUT)
    {
        unsigned long key = minitel.getKeyCode(false);
        if (key != 0)
        {
            switch (key)
            {
            case TOUCHE_FLECHE_GAUCHE:
                break;

            case TOUCHE_FLECHE_DROITE:
                break;

            case ENVOI:
                if (stringEnteredByUser.length() > 0)
                {
                    currentStep = STEP_HTTP_REQUEST;
                }
                break;

            default:
                String keyAsString = minitel.getString(key);
                minitel.print(keyAsString);
                stringEnteredByUser += keyAsString;
                break;
            }
        }
    }

    if (currentStep == STEP_HTTP_REQUEST)
    {
        displayNewPage("3615 Wikipedia");
        minitel.print("Performing search on Wikipedia...");

        if (WiFi.status() == WL_CONNECTED)
        {
            HTTPClient client;
            String serverPath = serverPathPart1 + stringEnteredByUser + serverPathPart2;

            client.begin(serverPath.c_str());

            int httpResponseCode = client.GET();

            if (httpResponseCode > 0)
            {
                String result = client.getString();

                deserializeJson(jsonDocument, result);
                wikipediaPage = findNestedKey(jsonDocument.as<JsonObject>(), "extract").as<String>();

                pageSeparators.clear();
                pageSeparators.push_back(0);
                currentPageSeparatorsIndex = 0;

                displayWikipediaPage(currentPageSeparatorsIndex);
            }
        }
    }

    if (currentStep == STEP_DISPLAY_RESULT)
    {
        unsigned long key = minitel.getKeyCode(false);
        if (key != 0)
        {
            switch (key)
            {
            case SOMMAIRE:
                displayPromptPage();
                break;

            case SUITE:
                displayWikipediaPage(currentPageSeparatorsIndex + 1);
                break;

            case RETOUR:
                displayWikipediaPage(currentPageSeparatorsIndex - 1);
                break;

            default:
                break;
            }
        }
    }
}
