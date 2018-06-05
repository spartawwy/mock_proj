#include "graphic_componet.h"
#include <QtWidgets/QApplication>

#include <qtextcodec.h>

int main(int argc, char *argv[])
{
    QCoreApplication::addLibraryPath("D:\\Qt\\qt5.2.1_win32\\bin\\plugins");

    QCoreApplication::addLibraryPath(".");
    QCoreApplication::addLibraryPath("./plugins");
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));

    QApplication a(argc, argv);
    GraphicComponet w;
    w.show();
    return a.exec();
}
