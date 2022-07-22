#include "mainwindow.h"
#include <QTranslator>
#include <QApplication>
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTranslator t;
    bool ok=t.load("lang.qm",":/lang");
    if(ok){
        a.installTranslator(&t);
    }
    MainWindow w;
    w.show();
    return a.exec();
}
