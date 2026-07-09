#ifndef DIDYOUMEANSCREEN_H
#define DIDYOUMEANSCREEN_H

#include "Screen.h"

class DidYouMeanScreen : public Screen
{
public:
    explicit DidYouMeanScreen(WikipediaApplication& application);

    void setup() override;
    bool update(unsigned long key) override;
};

#endif
