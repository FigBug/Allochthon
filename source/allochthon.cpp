#include "mainheaders.h"
#include "allochthon.h"
#include "redditbrowser.h"
#include "../lib/qjson-qjson/src/parser.h"

Allochthon::Allochthon(QWidget *parent, Qt::WFlags flags)
  : QMainWindow(parent, flags),
    netManager(NULL),
	pendingReply(NULL),
	timerId(0)
{
	ui.setupUi(this);

	ui.centralWidget->setLayout(ui.mainLayout);
	ui.mainLayout->setContentsMargins(QMargins(10, 10, 10, 10));

	setWindowIcon(QIcon(":/allochthon/icon.png"));

	restoreLayout();

	loadReddits();

	netManager = new QNetworkAccessManager(this);
	netManager->setCookieJar(cookies = new QNetworkCookieJar());

	connect(ui.add, SIGNAL(clicked()), this, SLOT(addReddit()));
	connect(ui.remove, SIGNAL(clicked()), this, SLOT(removeReddit()));
	connect(ui.reddits, SIGNAL(itemSelectionChanged()), this, SLOT(redditSelectionChanged()));	
	connect(netManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
	connect(ui.tabs, SIGNAL(tabCloseRequested(int)), this, SLOT(tabCloseRequested(int)));
	connect(ui.actionClose_Tab, SIGNAL(triggered()), this, SLOT(closeTab()));
	connect(ui.actionAbout_Allochthon, SIGNAL(triggered()), this, SLOT(aboutbox()));
	connect(ui.actionClear_Queue, SIGNAL(triggered()), this, SLOT(clearQueue()));
	connect(ui.actionClear_Queue_Mark, SIGNAL(triggered()), this, SLOT(clearQueueMark()));
	connect(ui.actionUndo_Closed_Tab, SIGNAL(triggered()), this, SLOT(undoCloseTab()));

	ui.statusBar->addWidget(status = new QLabel());

	progress = new QProgressBar(this);
	progress->setMaximumWidth(175);
	progress->setRange(0, 0);
	progress->setVisible(false);

	ui.statusBar->addPermanentWidget(progress);

	updateStatusBar();
	updateButtonStatus();
}

Allochthon::~Allochthon()
{

}

void Allochthon::restoreLayout()
{
	QSettings settings;
	QVariant rc = settings.value("uilayout/reviewGeometry");
	if (rc.isValid())
	{
		setGeometry(rc.toRect());
		if (settings.value("uilayout/reviewMaximized").toBool())
			showMaximized();
	}
	else
	{
		resize(800, 600);
	}
}

void Allochthon::closeEvent(QCloseEvent* event)
{
	delete netManager;

    QSettings settings;
	settings.setValue("uilayout/reviewGeometry", geometry());
	settings.setValue("uilayout/reviewMaximized", isMaximized());

	saveReddits();
}

void Allochthon::loadReddits()
{
	QStringList reddits = QSettings().value("reddits/list").toStringList();
	if (reddits.isEmpty())
		reddits = QStringList() << "pics" << "funny" << "adviceanimals" << "geek" << "bicycling" << "wtf";

	qSort(reddits);

	ui.reddits->clear();
	ui.reddits->addItems(reddits);

	loadHistory();
}

void Allochthon::saveReddits()
{
	QStringList items;

	for (int i = 0; i < ui.reddits->count(); i++)
		items.append(ui.reddits->item(i)->text());

	QSettings().setValue("reddits/list", items);	
	saveHistory();
}

void Allochthon::addReddit()
{
	QString reddit = QInputDialog::getText(this, "Sub Reddit", "Sub Reddit:");

	if (!reddit.isEmpty())
	{
		ui.reddits->addItem(reddit);
		ui.reddits->sortItems();
	}
	updateButtonStatus();
}

void Allochthon::removeReddit()
{
	QListWidgetItem* item = ui.reddits->currentItem();
	ui.reddits->setCurrentItem(NULL);
	if (item)
		delete item;

	updateButtonStatus();
}

void Allochthon::redditSelectionChanged()
{
	clearQueue();
	cancelRequest();

	QListWidgetItem* item = ui.reddits->currentItem();
	QString reddit = item ? item->text() : QString();

	if (!reddit.isEmpty())
		browseReddit(reddit);

	updateButtonStatus();	
}

void Allochthon::browseReddit(QString reddit)
{
	QUrl url(QString("http://www.reddit.com/r/%1/.json?limit=100").arg(reddit));

	QNetworkReply* reply = netManager->get(QNetworkRequest(url));	

	pendingReply = reply;

	progress->setVisible(true);
}

void Allochthon::replyFinished(QNetworkReply* reply)
{
	QByteArray json = reply->readAll();

	QJson::Parser parser;

	bool ok = false;
	QVariant result = parser.parse(json, &ok);

	if (ok)
		processResponse(result.toMap());
	
	reply->deleteLater();

	pendingReply = NULL;

	progress->setVisible(false);
}

void Allochthon::processResponse(QVariantMap doc)
{	
	QVariantMap data = doc["data"].toMap();

	QVariantList children = data["children"].toList();

	for (int i = 0; i < children.count(); i++)
	{
		QVariantMap child = children[i].toMap();
		QVariantMap data  = child["data"].toMap();

		RedditStory story(data["url"].toString(),
			              data["id"].toString(),
						  data["created_utc"].toDouble(),
						  data["title"].toString(),
						  data["permalink"].toString());

		stories.append(story);		
	}
	handleStories();
}

void Allochthon::handleStories()
{
	while (ui.tabs->count() < 5 && stories.count() > 0)
	{
		RedditStory story = stories.takeFirst();
		if (!history.contains(story.id))
		{			
			openStory(story);
		}
	}	
}

void Allochthon::openStory(RedditStory story, int pos, bool activate)
{
	RedditBrowser* rb;

	int idx = ui.tabs->insertTab(pos, rb = new RedditBrowser(story, history), story.title);
	ui.tabs->setIconSize(QSize(16,16));

	if (activate)
		ui.tabs->setCurrentIndex(idx);

	connect(rb, SIGNAL(loadStarted()), this, SLOT(loadStarted()));
	connect(rb, SIGNAL(loadFinished()), this, SLOT(loadFinished()));

	updateStatusBar();
}

void Allochthon::tabCloseRequested(int index)
{
	RedditBrowser* page = dynamic_cast<RedditBrowser*>(ui.tabs->widget(index));

	closedStories.append(page->getStory());

	ui.tabs->removeTab(index);
	delete page;

	handleStories();

	if (ui.tabs->count() == 0 || (ui.tabs->count() + stories.count()) % 20 == 0)
		saveHistory();

	updateStatusBar();
}

void Allochthon::closeTab()
{
	int idx = ui.tabs->currentIndex();
	if (idx >= 0)
		tabCloseRequested(idx);
}

void Allochthon::undoCloseTab()
{
	if (closedStories.size() > 0)
	{
		RedditStory story = closedStories.takeLast();

		openStory(story, 0, true);
	}
}

void Allochthon::aboutbox()
{
	QMessageBox::information(this, "Allochthon", "Allochthon by Roland Rabien (figbug@gmail.com)\n\nVersion: " VERSION);
}

void Allochthon::clearQueue()
{
	ui.tabs->clear();
	stories.clear();

	updateStatusBar();
	saveHistory();
}

void Allochthon::clearQueueMark()
{
	for (int i = 0; i < ui.tabs->count(); i++)
	{
		RedditBrowser* br = dynamic_cast<RedditBrowser*>(ui.tabs->widget(i));
		br->markAsRead();
	}

	for (int i = 0; i < stories.count(); i++)
	{
		RedditStory story = stories[i];
		history[story.id] = QVariant(story.pubDate);
	}

	clearQueue();
	saveHistory();
}

void Allochthon::updateStatusBar()
{
	status->setText(QString("To View: %1").arg(ui.tabs->count() + stories.count()));
}

void Allochthon::updateButtonStatus()
{
	ui.remove->setEnabled(ui.reddits->currentItem() != NULL);
}

void Allochthon::cancelRequest()
{
	if (pendingReply)
	{
		pendingReply->abort();
		pendingReply->deleteLater();
		pendingReply = NULL;

		progress->setVisible(true);
	}
}

void Allochthon::saveHistory()
{
	QDir dataDir = QDir(QDesktopServices::storageLocation(QDesktopServices::DataLocation));
	dataDir.mkpath(dataDir.absolutePath());
	QString historyFile = dataDir.filePath("history.dat");

	QFile file(historyFile);
	file.open(QIODevice::WriteOnly);
	QDataStream out(&file);

	// Write a header with a "magic number" and a version
	out << (quint32)0xC1E4A356;
	out << (quint32)1;
	out << (quint32)0; // flags

	out.setVersion(QDataStream::Qt_4_6);

	out << history;
}

void Allochthon::loadHistory()
{
	QString historyFile = QDir(QDesktopServices::storageLocation(QDesktopServices::DataLocation)).filePath("history.dat");
	if (QFileInfo(historyFile).exists())
	{
		QFile file(historyFile);
		file.open(QIODevice::ReadOnly);
		QDataStream in(&file);

		// Write a header with a "magic number" and a version
		quint32 magic;
		quint32 ver;
		quint32 flags;

		in >> magic;
		in >> ver;
		in >> flags;

		in.setVersion(QDataStream::Qt_4_6);

		in >> history;
	}
}

void Allochthon::loadFinished()
{
}

void Allochthon::loadStarted()
{
	if (timerId == 0)
		timerId = startTimer(30);
}

void Allochthon::timerEvent(QTimerEvent* event)
{
	int browsersLoading = 0;

	for (int i = 0; i < ui.tabs->count(); i++)
	{
		RedditBrowser* rb = dynamic_cast<RedditBrowser*>(ui.tabs->widget(i));

		if (rb->isLoading())
		{
			int frame = rb->getFrame() + 1;
			if (frame >= spinner.getNumFrames())
				frame = 0;

			ui.tabs->setTabIcon(i, spinner.getFrame(frame));
			rb->setFrame(frame);

			browsersLoading++;
		}
		else if (!rb->isLoading() && rb->getFrame() >= 0)
		{
			ui.tabs->setTabIcon(i, QIcon());
			rb->setFrame(-1);
		}
	}

	if (browsersLoading == 0)
	{
		killTimer(timerId);
		timerId = 0;
	}
}