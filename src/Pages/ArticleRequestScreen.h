#ifndef ARTICLEREQUESTSCREEN_H
#define ARTICLEREQUESTSCREEN_H

#include "Screen.h"

class ArticleRequestScreen : public Screen
{
public:
    explicit ArticleRequestScreen(WikipediaApplication& application);

    void setup() override;
    bool update(unsigned long key) override;
};

#endif
