#include "mainheaders.h"
#include "redditbrowser.h"

RedditBrowser::RedditBrowser(RedditStory story_, QVariantHash& history_)
  : QWidget(NULL),
	story(story_),
	showingComments(false),
	firstShow(true),
	history(history_),
	loading(false),
	frame(-1)
{
	webview = new QWebView(this);	

	url = new QLineEdit(this);
	toggle = new QPushButton("Show Comments", this);
	openInBrowser = new QPushButton("Open In Browser", this);

	QBoxLayout* topLayout = new QBoxLayout(QBoxLayout::LeftToRight);
	topLayout->addWidget(url);
	topLayout->addWidget(toggle);
	topLayout->addWidget(openInBrowser);	

	QBoxLayout* mainLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);
	mainLayout->addLayout(topLayout);
	mainLayout->addWidget(webview);

	setLayout(mainLayout);

	connect(toggle, SIGNAL(clicked()), this, SLOT(switchView()));
	connect(openInBrowser, SIGNAL(clicked()), this, SLOT(openBrowser()));
	connect(webview, SIGNAL(loadFinished(bool)), this,  SLOT(loadFinishedInt(bool)));
	connect(webview, SIGNAL(loadStarted()), this, SLOT(loadStartedInt()));
	connect(webview, SIGNAL(urlChanged(const QUrl&)), this, SLOT(urlChanged(const QUrl&)));

	webview->load(QUrl(story.link));
	url->setText(story.link);
}

RedditBrowser::~RedditBrowser()
{
}

void RedditBrowser::switchView()
{
	QString urlText = showingComments ? story.link : "http://www.reddit.com" + story.comments;

	webview->load(QUrl(urlText));
	url->setText(urlText);

	showingComments = !showingComments;

	toggle->setText(showingComments ? "Show Story" : "Show Comments");
}

void RedditBrowser::resizeEvent(QResizeEvent* event)
{
}

void RedditBrowser::showEvent(QShowEvent* event)
{
	if (firstShow)
	{
		markAsRead();		
		firstShow = false;
	}
}

void RedditBrowser::markAsRead()
{
	history[story.id] = QVariant(story.pubDate);
}

void RedditBrowser::loadFinishedInt(bool ok)
{
	loading = false;
	emit loadFinished();	
}

void RedditBrowser::loadStartedInt()
{
	loading = true;
	emit loadStarted();
}

void RedditBrowser::urlChanged(const QUrl& u)
{
	url->setText(u.toString());
}

void RedditBrowser::openBrowser()
{
	QDesktopServices::openUrl(QUrl(url->text()));
}

int RedditBrowser::getFrame()
{
	return frame;
}

void RedditBrowser::setFrame(int frame_)
{
	frame = frame_;
}


bool RedditBrowser::isLoading()
{
	return loading;
}

RedditStory RedditBrowser::getStory()
{
	return story;
}