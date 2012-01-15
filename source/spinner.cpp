#include "mainheaders.h"
#include "spinner.h"

Spinner::Spinner()
{
	allFrames.load(":/allochthon/spinner.png");
}

int Spinner::getNumFrames()
{
	return 8 * 4 - 1;
}

QIcon Spinner::getFrame(int frame)
{
	int x = (frame + 1) % 8;
	int y = (frame + 1) / 8;

	return QIcon(allFrames.copy(x * 16, y * 16, 16, 16));
}