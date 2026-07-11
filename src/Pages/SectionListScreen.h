#ifndef SECTIONLISTSCREEN_H
#define SECTIONLISTSCREEN_H

#include <vector>

#include <MinitelToolkit.h>

#include "Screen.h"

class SectionListScreen : public Screen
{
public:
    explicit SectionListScreen(WikipediaApplication& application);

    void setup() override;
    bool update(unsigned long key) override;

private:
    int sectionIndexFor(const TreeItem* item) const;

    TreeViewModelPtr sectionTreeModel;
    TreeViewPtr sectionTreeView;
    std::vector<TreeItem*> itemForSection; // Non-owning, aligned to article section indices.
};

#endif
