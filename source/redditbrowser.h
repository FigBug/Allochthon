#pragma once

#include "RedditStory.h"

class RedditBrowser : public QWidget
{
	Q_OBJECT

public:
	RedditBrowser(RedditStory story);
	~RedditBrowser();

public slots:
	void switchView();

private slots:
	void loadFinished(bool ok);
	void loadStarted();
	void urlChanged(const QUrl&);

protected:
	void resizeEvent(QResizeEvent* event);

private:
	RedditStory story;
	bool showingComments;

	QWebView* webview;
	QLineEdit* url;
	QPushButton* toggle;
	QProgressBar* progress;
};