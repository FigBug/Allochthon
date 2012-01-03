#pragma once

class RedditStory
{
public:
	RedditStory(QString link, QString id, double pubDate, QString title, QString comments);

	QString link;
	QString id;
	double pubDate;
	QString title;
	QString comments;
};