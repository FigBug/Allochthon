#pragma once

#include "RedditStory.h"

class RedditBrowser : public QWidget
{
	Q_OBJECT

public:
	RedditBrowser(RedditStory story, QVariantHash& history);
	~RedditBrowser();

	bool isLoading();
	int getFrame();
	void setFrame(int frame);
	RedditStory getStory();

signals:
	void loadFinished();	
	void loadStarted();

public slots:
	void switchView();
	void markAsRead();

private slots:
	void loadFinishedInt(bool ok);
	void loadStartedInt();
	void urlChanged(const QUrl&);
	void openBrowser();

protected:
	void resizeEvent(QResizeEvent* event);
	void showEvent(QShowEvent* event);	

private:
	RedditStory story;
	bool showingComments;
	bool firstShow;
	bool loading;
	int frame;

	QWebView* webview;
	QLineEdit* url;
	QPushButton* toggle;
	QPushButton* openInBrowser;	
	QVariantHash& history;
};