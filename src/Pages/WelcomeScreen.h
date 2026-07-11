#ifndef WELCOMESCREEN_H
#define WELCOMESCREEN_H

#include <Arduino.h>

#include "Screen.h"

class WelcomeScreen : public Screen
{
public:
    explicit WelcomeScreen(WikipediaApplication& application);

    void setup() override;
    bool update(unsigned long key) override;

private:
    String stringEnteredByUser;
};

#endif
