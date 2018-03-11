#include "flashingorder.h"

#include <QtWidgets/QApplication>
#include <qtextcodec.h>

int main(int argc, char *argv[])
{
	QCoreApplication::addLibraryPath("D:\\Qt\\qt5.2.1_win32\\bin\\plugins");
 
	QCoreApplication::addLibraryPath(".");
	QCoreApplication::addLibraryPath("./plugins");
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK")); 

	QApplication a(argc, argv);
	// todo: use argv to get account no and password
	FlashingOrder &flashing_order = AppInstance();
	
	flashing_order.Init(argc, argv);
	flashing_order.show();

	return a.exec();
}

