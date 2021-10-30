#include "mainwindow.h"

#include <QApplication>
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
int WinMain(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
