#ifndef RESULTSELECTIONSCREEN_H
#define RESULTSELECTIONSCREEN_H

#include <MinitelToolkit.h>

#include "Screen.h"

class ResultSelectionScreen : public Screen
{
public:
    explicit ResultSelectionScreen(WikipediaApplication& application);

    void setup() override;
    bool update(unsigned long key) override;

private:
    TableViewModelPtr searchResultsModel;
    TableViewPtr searchResultsView;
};

#endif
