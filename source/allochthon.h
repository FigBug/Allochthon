#pragma once

#include "ui_allochthon.h"

#include "redditstory.h"
#include "spinner.h"

class Allochthon : public QMainWindow
{
	Q_OBJECT

public:
	Allochthon(QWidget *parent = 0, Qt::WFlags flags = 0);
	~Allochthon();

private slots:
	void addReddit();
	void removeReddit();
	void redditSelectionChanged();
	void replyFinished(QNetworkReply* reply);
	void handleStories();
	void tabCloseRequested(int index);
	void closeTab();
	void aboutbox();
	void clearQueue();
	void clearQueueMark();
	void loadFinished();	
	void loadStarted();
	void undoCloseTab();

protected:
	void closeEvent(QCloseEvent* event);		
	void timerEvent(QTimerEvent* event);

private:
	void restoreLayout();
	void loadReddits();
	void saveReddits();		
	void browseReddit(QString reddit);	
	void processResponse(QVariantMap doc);	
	void updateStatusBar();
	void updateButtonStatus();
	void cancelRequest();
	void saveHistory();
	void loadHistory();
	void openStory(RedditStory story, int pos = -1, bool activate = false);

	Ui::allochthonClass ui;

	QNetworkAccessManager* netManager;
	QNetworkCookieJar* cookies;

	QVariantHash history;
	QList<RedditStory> stories;
	QList<RedditStory> closedStories;

	QLabel* status;
	QProgressBar* progress;
	Spinner spinner;

	QNetworkReply* pendingReply;

	int timerId;
};
