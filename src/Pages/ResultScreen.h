#ifndef RESULTSCREEN_H
#define RESULTSCREEN_H

#include "Screen.h"

class ResultScreen : public Screen
{
public:
    explicit ResultScreen(WikipediaApplication& application);

    void setup() override;
    bool update(unsigned long key) override;

private:
    void showResultPage();
    void showLoadingHint();
};

#endif
