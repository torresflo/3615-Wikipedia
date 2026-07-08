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
        int index; // Section index, used to fetch its content on demand
    };

    void clear();
    bool isEmpty() const;

    const String& getTitle() const;
    const String& getDescription() const;
    const std::vector<Section>& getSections() const;

    void setTitle(String&& value);
    void setDescription(String&& value);
    void addSection(String&& title, int level, int index);

    void printToSerial() const;

private:
    String title;
    String description;
    std::vector<Section> sections;
};

#endif
