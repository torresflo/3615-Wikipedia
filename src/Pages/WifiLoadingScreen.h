#ifndef WIFILOADINGSCREEN_H
#define WIFILOADINGSCREEN_H

#include "Screen.h"

class WifiLoadingScreen : public Screen
{
public:
    explicit WifiLoadingScreen(WikipediaApplication& application);

    void setup() override;
    bool update(unsigned long key) override;

private:
    static const char* ssid;
    static const char* password;
};

#endif
