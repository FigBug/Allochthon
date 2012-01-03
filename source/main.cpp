#include "mainheaders.h"
#include "allochthon.h"

int main(int argc, char *argv[])
{
	QCoreApplication::setOrganizationName("figbug");
	QCoreApplication::setOrganizationDomain("com.figbug");
	QCoreApplication::setApplicationName("Allochthon");
	QCoreApplication::setApplicationVersion("1.0.0.0");	

	QApplication a(argc, argv);
	Allochthon w;
	w.show();
	return a.exec();
}
