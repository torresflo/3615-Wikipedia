#include "SectionListScreen.h"

#include <memory>
#include <utility>
#include <vector>

#include "WikipediaApplication.h"

SectionListScreen::SectionListScreen(WikipediaApplication& application)
    : Screen(application)
{}

void SectionListScreen::setup()
{
    ExtendedMinitelPtr& minitel = application.getMinitel();
    const WikipediaArticle& article = application.getClient().getArticle();
    const std::vector<WikipediaArticle::Section>& sections = article.getSections();

    minitel->displayNewPage(article.getTitle());

    sectionTreeModel = std::make_shared<TreeViewModel>(article.getTitle());
    itemForSection.assign(sections.size(), nullptr);

    std::vector<std::pair<int, TreeItem*>> ancestors;
    for (size_t i = 0; i < sections.size(); ++i)
    {
        const WikipediaArticle::Section& section = sections[i];

        while (!ancestors.empty() && ancestors.back().first >= section.level)
            ancestors.pop_back();

        TreeItem* parent = ancestors.empty() ? sectionTreeModel->root() : ancestors.back().second;

        std::unique_ptr<TreeItem> item(new TreeItem(section.title, parent));
        TreeItem* itemPointer = item.get();
        parent->appendChild(std::move(item));

        itemForSection[i] = itemPointer;
        ancestors.push_back({section.level, itemPointer});
    }

    sectionTreeView = std::make_shared<TreeView>(sectionTreeModel.get(), 1, 3);
    sectionTreeView->setMaxWidth(38);
    sectionTreeView->draw(minitel);

    minitel->displayString("FLECHES to move, ENVOI to read", 1, 23);
    minitel->displayString("GUIDE back, SOMMAIRE new search", 1, 24);
}

bool SectionListScreen::update(unsigned long key)
{
    if (key == 0)
        return true;

    ExtendedMinitelPtr& minitel = application.getMinitel();

    switch (key)
    {
    case GUIDE:
        application.setNextScreenId(ScreenId::Result);
        return false;

    case ENVOI:
    {
        ModelIndex index = sectionTreeView->getModelIndexUnderCursor();
        const TreeItem* selected = static_cast<const TreeItem*>(index.getInternalPointer());
        int sectionIndex = sectionIndexFor(selected);
        if (sectionIndex >= 0)
        {
            minitel->displayString("Loading...",  1, 3);
            application.getResultPager().goToSection(sectionIndex);
            application.setNextScreenId(ScreenId::Result);
            return false;
        }
        break;
    }

    case TOUCHE_FLECHE_HAUT:
        if (sectionTreeView->moveVirtualCursor(CursorDirection::Up))
            sectionTreeView->drawVirtualCursor(minitel);
        break;

    case TOUCHE_FLECHE_BAS:
        if (sectionTreeView->moveVirtualCursor(CursorDirection::Down))
            sectionTreeView->drawVirtualCursor(minitel);
        break;

    case TOUCHE_FLECHE_DROITE:
        if (sectionTreeView->moveVirtualCursor(CursorDirection::Right))
            sectionTreeView->draw(minitel);
        break;

    case TOUCHE_FLECHE_GAUCHE:
        if (sectionTreeView->moveVirtualCursor(CursorDirection::Left))
            sectionTreeView->draw(minitel);
        break;

    default:
        break;
    }

    return true;
}

int SectionListScreen::sectionIndexFor(const TreeItem* item) const
{
    if (item == nullptr)
        return -1;

    for (size_t i = 0; i < itemForSection.size(); ++i)
    {
        if (itemForSection[i] == item)
            return static_cast<int>(i);
    }

    return -1;
}
