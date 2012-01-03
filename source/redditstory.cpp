#include "mainheaders.h"
#include "redditstory.h"

RedditStory::RedditStory(QString link_, QString id_, double pubDate_, QString title_, QString comments_)
  : link(link_),
    id(id_),
    pubDate(pubDate_),
    title(title_),
	comments(comments_)
{
}