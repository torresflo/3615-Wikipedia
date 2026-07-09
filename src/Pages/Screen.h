#ifndef SCREEN_H
#define SCREEN_H

class WikipediaApplication;

enum class ScreenId
{
    WifiLoading,
    UserInput,
    SearchRequest,
    ResultSelection,
    DidYouMean,
    ArticleRequest,
    Result
};

class Screen
{
public:
    explicit Screen(WikipediaApplication& application) : application(application) {}
    virtual ~Screen() = default;

    virtual void setup() = 0;
    virtual bool update(unsigned long key) = 0;  // true = stay on this screen, false = finished.

protected:
    WikipediaApplication& application;
};

#endif
