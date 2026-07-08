#ifndef WIKIPEDIAARTICLE_H
#define WIKIPEDIAARTICLE_H

#include <vector>

#include <Arduino.h>

class WikipediaArticle
{
public:
    struct Section
    {
        String title;
        int level;
        String content;
    };

    void clear();
    bool isEmpty() const;

    const String& getTitle() const;
    const String& getDescription() const;
    const String& getSummary() const;
    const std::vector<Section>& getSections() const;
    const std::vector<String>& getCategories() const;

    void setTitle(String&& value);
    void setDescription(String&& value);
    void setSummary(String&& value);
    void addSection(String&& title, int level, String&& content);
    void addCategory(String&& category);

    void printToSerial() const;

private:
    String title;
    String description;
    String summary;
    std::vector<Section> sections;
    std::vector<String> categories;
};

#endif
