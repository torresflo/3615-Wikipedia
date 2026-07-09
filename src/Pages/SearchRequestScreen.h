#ifndef SEARCHREQUESTSCREEN_H
#define SEARCHREQUESTSCREEN_H

#include "Screen.h"

class SearchRequestScreen : public Screen
{
public:
    explicit SearchRequestScreen(WikipediaApplication& application);

    void setup() override;
    bool update(unsigned long key) override;
};

#endif
