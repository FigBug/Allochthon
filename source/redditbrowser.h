#pragma once

#include "RedditStory.h"

class RedditBrowser : public QWebView
{
	Q_OBJECT

public:
	RedditBrowser(RedditStory story);
	~RedditBrowser();

	void switchView();

private:
	RedditStory story;
	bool showingComments;
};