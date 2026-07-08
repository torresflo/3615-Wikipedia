#include "WikipediaArticle.h"

#include <utility>

#include <MinitelToolkit.h>

void WikipediaArticle::clear()
{
    title = String();
    description = String();
    sections.clear();
}

bool WikipediaArticle::isEmpty() const
{
    return title.length() == 0 && sections.empty();
}

const String& WikipediaArticle::getTitle() const
{
    return title;
}

const String& WikipediaArticle::getDescription() const
{
    return description;
}

const std::vector<WikipediaArticle::Section>& WikipediaArticle::getSections() const
{
    return sections;
}

void WikipediaArticle::setTitle(String&& value)
{
    title = std::move(value);
}

void WikipediaArticle::setDescription(String&& value)
{
    description = std::move(value);
}

void WikipediaArticle::addSection(String&& title, int level, int index)
{
    sections.push_back(Section{std::move(title), level, index});
}

void WikipediaArticle::printToSerial() const
{
    debugPrintf("===== Wikipedia Article =====\n");
    debugPrintf("Title: %s\n", title.c_str());
    debugPrintf("Description: %s\n", description.c_str());

    debugPrintf("\n--- Sections (%u) ---\n", sections.size());
    for (const Section& section : sections)
    {
        debugPrintf("[L%d] (#%d) %s\n", section.level, section.index, section.title.c_str());
    }

    debugPrintf("=============================\n");
}
