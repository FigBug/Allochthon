#include "mainheaders.h"
#include "allochthon.h"
#include "redditbrowser.h"
#include "../lib/qjson-qjson/src/parser.h"

Allochthon::Allochthon(QWidget *parent, Qt::WFlags flags)
  : QMainWindow(parent, flags),
    netManager(NULL)
{
	ui.setupUi(this);

	ui.centralWidget->setLayout(ui.mainLayout);
	ui.mainLayout->setContentsMargins(QMargins(10, 10, 10, 10));

	restoreLayout();

	loadReddits();

	netManager = new QNetworkAccessManager(this);

	connect(ui.add, SIGNAL(clicked()), this, SLOT(addReddit()));
	connect(ui.remove, SIGNAL(clicked()), this, SLOT(removeReddit()));
	connect(ui.reddits, SIGNAL(itemSelectionChanged()), this, SLOT(redditSelectionChanged()));	
	connect(netManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
	connect(ui.tabs, SIGNAL(tabCloseRequested(int)), this, SLOT(tabCloseRequested(int)));
	connect(ui.actionClose_Tab, SIGNAL(triggered()), this, SLOT(closeTab()));
	connect(ui.actionSwitch_Tab_View, SIGNAL(triggered()), this, SLOT(switchView()));
	connect(ui.actionAbout_Allochthon, SIGNAL(triggered()), this, SLOT(aboutbox()));
	connect(ui.actionClear_Queue, SIGNAL(triggered()), this, SLOT(clearQueue()));

	ui.statusBar->addWidget(status = new QLabel());

	updateStatusBar();
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

	history = QSettings().value("reddits/history").toHash();
}

void Allochthon::saveReddits()
{
	QStringList items;

	for (int i = 0; i < ui.reddits->count(); i++)
		items.append(ui.reddits->item(i)->text());

	QSettings().setValue("reddits/list", items);
	QSettings().setValue("reddits/history", history);
}

void Allochthon::addReddit()
{
	QString reddit = QInputDialog::getText(this, "Sub Reddit", "Sub Reddit:");

	if (!reddit.isEmpty())
	{
		ui.reddits->addItem(reddit);
		ui.reddits->sortItems();
	}
}

void Allochthon::removeReddit()
{
	QListWidgetItem* item = ui.reddits->currentItem();
	ui.reddits->setCurrentItem(NULL);
	if (item)
		delete item;
}

void Allochthon::redditSelectionChanged()
{
	QListWidgetItem* item = ui.reddits->currentItem();
	QString reddit = item ? item->text() : QString();

	if (!reddit.isEmpty())
		browseReddit(reddit);
}

void Allochthon::browseReddit(QString reddit)
{
	QUrl url(QString("http://www.reddit.com/r/%1/.json?limit=100").arg(reddit));

	QNetworkReply* reply = netManager->get(QNetworkRequest(url));	
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
			history[story.id] = QVariant(story.pubDate);

			ui.tabs->addTab(new RedditBrowser(story), story.title);
		}
	}	
	updateStatusBar();
}

void Allochthon::tabCloseRequested(int index)
{
	ui.tabs->removeTab(index);

	handleStories();
}

void Allochthon::closeTab()
{
	int idx = ui.tabs->currentIndex();
	if (idx >= 0)
		tabCloseRequested(idx);
}

void Allochthon::switchView()
{
	int idx = ui.tabs->currentIndex();
	if (idx >= 0)
	{
		RedditBrowser* browser = dynamic_cast<RedditBrowser*>(ui.tabs->widget(idx));

		if (browser)
			browser->switchView();
	}
}

void Allochthon::aboutbox()
{
	QMessageBox::information(this, "Allochthon", "Allochthon by Roland Rabien (figbug@gmail.com)");
}

void Allochthon::clearQueue()
{
	stories.clear();

	updateStatusBar();
}

void Allochthon::updateStatusBar()
{
	status->setText(QString("To View: %1").arg(ui.tabs->count() + stories.count()));
}