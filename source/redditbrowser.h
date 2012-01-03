#pragma once

#include "RedditStory.h"

class RedditBrowser : public QWidget
{
	Q_OBJECT

public:
	RedditBrowser(RedditStory story, QVariantHash& history);
	~RedditBrowser();

public slots:
	void switchView();

private slots:
	void loadFinished(bool ok);
	void loadStarted();
	void urlChanged(const QUrl&);
	void openBrowser();

protected:
	void resizeEvent(QResizeEvent* event);
	void showEvent(QShowEvent* event);

private:
	RedditStory story;
	bool showingComments;
	bool firstShow;

	QWebView* webview;
	QLineEdit* url;
	QPushButton* toggle;
	QPushButton* openInBrowser;
	QProgressBar* progress;
	QVariantHash& history;
};