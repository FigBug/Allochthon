#include "mainheaders.h"
#include "redditbrowser.h"

RedditBrowser::RedditBrowser(RedditStory story_)
  : QWidget(NULL),
	story(story_),
	showingComments(false)
{
	webview = new QWebView(this);
	url = new QLineEdit(this);
	toggle = new QPushButton("Toggle Comments", this);
	progress = new QProgressBar(this);
	progress->setMaximumWidth(75);

	QBoxLayout* topLayout = new QBoxLayout(QBoxLayout::LeftToRight);
	topLayout->addWidget(url);
	topLayout->addWidget(toggle);
	topLayout->addWidget(progress);

	QBoxLayout* mainLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);
	mainLayout->addLayout(topLayout);
	mainLayout->addWidget(webview);

	setLayout(mainLayout);

	connect(toggle, SIGNAL(clicked()), this, SLOT(switchView()));
	connect(webview, SIGNAL(loadFinished(bool)), this,  SLOT(loadFinished(bool)));
	connect(webview, SIGNAL(loadStarted()), this, SLOT(loadStarted()));
	connect(webview, SIGNAL(urlChanged(const QUrl&)), this, SLOT(urlChanged(const QUrl&)));

	progress->setValue(0);
	progress->setRange(0, 1);
	progress->setEnabled(false);
	progress->setTextVisible(false);
	progress->setVisible(false);

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
}

void RedditBrowser::resizeEvent(QResizeEvent* event)
{
}

void RedditBrowser::loadFinished(bool ok)
{
	progress->setValue(0);
	progress->setRange(0, 1);
	progress->setEnabled(false);
	progress->setVisible(false);
}

void RedditBrowser::loadStarted()
{
	progress->setValue(0);
	progress->setRange(0, 0);
	progress->setEnabled(true);
	progress->setVisible(true);
}

void RedditBrowser::urlChanged(const QUrl& u)
{
	url->setText(u.toString());
}
