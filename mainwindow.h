#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include<QPushButton>
#include"mazewidget.h" //子窗口头文件
class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

signals:

public slots:
       void changeWin();
private:
   QPushButton b;
   MazeWidget sub;
protected:
   void paintEvent(QPaintEvent*);

};

#endif // MAINWINDOW_H
