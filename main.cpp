#include "mazewidget.h"
#include"mainwindow.h"
#include <QApplication>
#include<QtMultimedia/QMediaPlayer>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    a.setWindowIcon(QIcon("win.ico"));
    return a.exec();
}
