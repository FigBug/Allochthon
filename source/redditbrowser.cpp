#include "mainheaders.h"
#include "redditbrowser.h"

RedditBrowser::RedditBrowser(RedditStory story_)
  : QWebView(NULL),
	story(story_),
	showingComments(false)
{
	load(QUrl(story.link));
}

RedditBrowser::~RedditBrowser()
{
}

void RedditBrowser::switchView()
{
	if (showingComments)
		load(QUrl(story.link));
	else
		load(QUrl("http://www.reddit.com" + story.comments));

	showingComments = !showingComments;
}