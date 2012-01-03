#pragma once

#include "ui_allochthon.h"

#include "redditstory.h"

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
	void switchView();
	void aboutbox();
	void clearQueue();

protected:
	void closeEvent(QCloseEvent* event);	

private:
	void restoreLayout();
	void loadReddits();
	void saveReddits();		
	void browseReddit(QString reddit);	
	void processResponse(QVariantMap doc);	

	Ui::allochthonClass ui;

	QNetworkAccessManager* netManager;

	QVariantHash history;
	QList<RedditStory> stories;
};
