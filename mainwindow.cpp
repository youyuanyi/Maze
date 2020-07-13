#include "mainwindow.h"
#include<QPainter>
#include <QSound>
MainWindow::MainWindow(QWidget *parent) : QWidget(parent)
{
         this->setWindowTitle("主界面");
         b.setParent(this);
         b.setGeometry(500,150,180,80);
         b.setStyleSheet("QPushButton{border-image:url(:/maze/image/startgame.jpg);border-radius:5px;}");
         QSound *sound=new QSound(":/maze/image/bgm.wav",this);
         sound->setLoops(-1);
         sound->play();
         this->resize(1200,900);
//         //显示子窗口
//         sub.show();
         connect(&b,&QPushButton::released,this,&MainWindow::changeWin);
}
void MainWindow::changeWin()
{
  //子窗口显示，本窗口隐藏
    sub.show();
    this->hide();
}
void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    //画背景图
    p.drawPixmap(0,0,width(),height(),QPixmap(":/maze/image/bg.jpg"));
}
