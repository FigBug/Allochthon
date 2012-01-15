#pragma once

class Spinner : public QObject
{
	Q_OBJECT
public:
	Spinner();

	int getNumFrames();

	QIcon getFrame(int frame);

private:
	QPixmap allFrames;
};