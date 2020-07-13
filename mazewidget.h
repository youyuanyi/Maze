#ifndef MAZEWIDGET_H
#define MAZEWIDGET_H
#include <QWidget>
#include <iostream>
#include <vector>
#include <queue>
#include <QTime>
#include <QLineEdit>
#include <QPushButton>
#include <QPainter>
#include <QLabel>
#include <QMessageBox>
#include <QDebug>
#include <QKeyEvent>
#include <QPixmap>
#include <QPen>
#include"stack.h"
using namespace std;
namespace Ui {
class MazeWidget;
}
class Maze //迷宫可看成许多个节点组成，每个节点保存了它的位置坐标和状态.0:墙；1：路：2：自动寻路路径
{
    public:
    int i,j;
    int state;
};
class Box1{ //这个是用来存放自动寻路时前驱节点的信息
public:
    int i,j;
    int pre;//上一位置的信息
    int num;
};

class MazeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MazeWidget(QWidget *parent = 0);
    ~MazeWidget();
    bool isJishu();
    void paintEvent(QPaintEvent *);//绘图事件,画迷宫
    void CreatMaze();//创建迷宫
    void keyPressEvent(QKeyEvent*);//键盘事件，每次按下一个键就是重写事件
    void FindShortWay();//寻找最短路径
    void timerEvent(QTimerEvent*);//定时器事件

    int score=0;
private:
    Ui::MazeWidget *ui;
    Maze **map;//用一个二维数组存放迷宫地图
    vector<Maze> *FinalPath;//定义一个向量用于存放迷宫路径
    Stack<Maze>*MazeStack;//定义一个栈用于存放迷宫节点
    Stack<Box1>*Box1Stack;//定义一个栈用于存放最短路径节点,在自动寻路时需要用到
    queue<Box1>*Box1Queue;//定义一个队列用于存放前驱
    int M,N;//迷宫的行列数
    bool first;//起点
    QLineEdit *Xline;//行编辑控件
    QLineEdit *Yline;//行编辑控件
    QLabel *tips;//说明提示的标签
    QLabel *recounts;//"倒计时"文字的标签
    QPushButton *creatmazebtn;//创建迷宫的按钮
    QPushButton *findshortwaybtn;//寻找最短路径的按钮
    QLabel *time; //显示时间的标签
    QLabel *toScore;//显示得分的标签
    QPushButton *hasPath;
    int X,Y;//海绵宝宝的坐标
    int timerId;//定时器ID
     bool find=false;//自动寻路时判断有没有到达终点
     bool flag=false;//判断有没有通路
//    static int sec;//时间
     int P,Q;//章鱼哥的坐标
     int randpos;//章鱼哥的位置随机出现
     int k=1;//积分系数
 private slots://槽函数
    void creatmazebtnSlot();
    void findshortwaybtnSlot();
    void counttimeSlot();
    void isHasPathSlot();


};

#endif // MAZEWIDGET_H
