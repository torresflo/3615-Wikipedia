#include "WikipediaArticle.h"

#include <utility>

#include <MinitelToolkit.h>

void WikipediaArticle::clear()
{
    title = String();
    description = String();
    summary = String();
    sections.clear();
    categories.clear();
}

bool WikipediaArticle::isEmpty() const
{
    return title.length() == 0 && summary.length() == 0;
}

const String& WikipediaArticle::getTitle() const
{
    return title;
}

const String& WikipediaArticle::getDescription() const
{
    return description;
}

const String& WikipediaArticle::getSummary() const
{
    return summary;
}

const std::vector<WikipediaArticle::Section>& WikipediaArticle::getSections() const
{
    return sections;
}

const std::vector<String>& WikipediaArticle::getCategories() const
{
    return categories;
}

void WikipediaArticle::setTitle(String&& value)
{
    title = std::move(value);
}

void WikipediaArticle::setDescription(String&& value)
{
    description = std::move(value);
}

void WikipediaArticle::setSummary(String&& value)
{
    summary = std::move(value);
}

void WikipediaArticle::addSection(String&& title, int level, String&& content)
{
    sections.push_back(Section{std::move(title), level, std::move(content)});
}

void WikipediaArticle::addCategory(String&& category)
{
    categories.push_back(std::move(category));
}

void WikipediaArticle::printToSerial() const
{
    debugPrintf("===== Wikipedia Article =====\n");
    debugPrintf("Title: %s\n", title.c_str());
    debugPrintf("Description: %s\n", description.c_str());

    debugPrintf("\n--- Summary ---\n");
    debugPrint(summary);
    debugPrint("\n");

    debugPrintf("\n--- Sections (%u) ---\n", sections.size());
    for (const Section& section : sections)
    {
        debugPrintf("[L%d] %s\n", section.level, section.title.c_str());
        debugPrint(section.content);
        debugPrint("\n");
    }

    debugPrintf("\n--- Categories (%u) ---\n", categories.size());
    for (const String& category : categories)
    {
        debugPrintf("- %s\n", category.c_str());
    }

    debugPrintf("=============================\n");
}
