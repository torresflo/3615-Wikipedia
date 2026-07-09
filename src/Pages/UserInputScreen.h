#ifndef USERINPUTSCREEN_H
#define USERINPUTSCREEN_H

#include <Arduino.h>

#include "Screen.h"

class UserInputScreen : public Screen
{
public:
    explicit UserInputScreen(WikipediaApplication& application);

    void setup() override;
    bool update(unsigned long key) override;

private:
    String stringEnteredByUser;
};

#endif
