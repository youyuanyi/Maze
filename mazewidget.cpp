#include "mazewidget.h"
#include "ui_mazewidget.h"
//int MazeWidget::sec=300;
MazeWidget::MazeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MazeWidget)
{

    ui->setupUi(this);
    first=true;
    M=33;
    N=33;//先预先设置迷宫大小
    X=3;
    Y=3;
    randpos=qrand()%25;
    if(randpos%2==0)
        randpos++;
    P=randpos;
    Q=randpos;
    this->setWindowTitle("寻找派大星");
    this->resize((N+10)*20,(M+5)*20);
    this->setFocus(Qt::MouseFocusReason);
    //接下来给控件们指定父对象(都是窗口)和对一些控件的位置进行设定
    Xline=new QLineEdit(this);
    Yline=new QLineEdit(this);
    Xline->setText("33");
    Yline->setText("33");
    Xline->setGeometry(30,(M+2)*20,40,20);
    Yline->setGeometry(90,(M+2)*20,40,20);
    //创建"开始游戏"控件
    creatmazebtn=new QPushButton(this);
    creatmazebtn->setText("开始游戏");
    creatmazebtn->setGeometry(150,(M+2)*20,120,30);
    creatmazebtn->setStyleSheet("QPushButton{background-color:yellow;}");
    //创建'寻找最短路径'控件
    findshortwaybtn=new QPushButton(this);
    findshortwaybtn->setText("最短路径");
    findshortwaybtn->setStyleSheet("QPushButton{background-color:rgb(250,128,114);}");
    findshortwaybtn->setGeometry((N+2)*20,30,100,30);
    findshortwaybtn->setEnabled(false);
    //创建提示标签
    tips=new QLabel(this);
    tips->setText("Tips:输入大于5的奇数");
    tips->setGeometry(330,(M+2)*20,190,30);
    tips->setStyleSheet("QLabel{background-color:rgb(122,154,136)};font-weight:bold;");
    //创建显示"倒计时"这三个字的标签
    recounts=new QLabel(this);
    QPixmap pixmap;
    pixmap.load(":maze/image/counttime.jpg");
    recounts->setPixmap(pixmap);
    recounts->setScaledContents(true);
    recounts->setGeometry((N+2)*20,90,120,50);

    //创建定时器标签
    time=new QLabel(this);

    time->setGeometry((N+2)*20,120,120,20);
    //创建得分的标签
    toScore=new QLabel(this);
    toScore->setGeometry((N+2)*20,150,120,20);
    toScore->setStyleSheet("QLabel{background-color:rgb(250,128,114);font-size:22px;color:white;border-radius:5px;}");
    toScore->setText(QString("得分:%1").arg(score*k));
    //
    hasPath=new QPushButton(this);
    hasPath->setGeometry((N+2)*20,220,120,30);
    hasPath->setText("没有通路");
    hasPath->setEnabled(false);
    hasPath->setStyleSheet("QPushButton{background-color:rgb(0,255,127);}");
    //先初始化迷宫，把所有节点的state都设置为0，即都是墙壁
    map=new Maze*[M];
    for(int i=0;i<M;i++)
        map[i]=new Maze[N];
    for(int i=0;i<M;i++)
    {
        for(int j=0;j<N;j++)
        {
            map[i][j].state=0;
            map[i][j].i=i;
            map[i][j].j=j;
        }
    }
    MazeStack=new Stack<Maze>;
    FinalPath=new vector<Maze>;
    Box1Stack=new Stack<Box1>;
    Box1Queue=new queue<Box1>;

    //接下来是自定义槽函数
    connect(creatmazebtn,&QPushButton::clicked,this,&MazeWidget::creatmazebtnSlot);//点击按钮生成迷宫的函数
    connect(findshortwaybtn,&QPushButton::clicked,this,&MazeWidget::findshortwaybtnSlot);//点击按钮寻找最短路径
//    connect(creatmazebtn,&QPushButton::clicked,this,&MazeWidget::counttimeSlot);
    connect(hasPath,&QPushButton::clicked,this,&MazeWidget::isHasPathSlot);

}

//判断是不是奇数
bool MazeWidget::isJishu()
{
    if(Xline->text().toInt()%2==0||Yline->text().toInt()%2==0)
        return false;
    return true;
}
//写绘图事件函数
void MazeWidget::paintEvent(QPaintEvent *)
{

    //创建画家对象,在MazeWidget窗口上绘图
    QPainter painter(this);
    //创建画笔
    QPen pen;
    pen.setColor(QColor(Qt::yellow));
    pen.setStyle(Qt::CustomDashLine);
    //把🖊给画家
    painter.setPen(pen);
    //找到路后再手动操作时道路发生的变化
    for(int i=FinalPath->size()-1;i>=0;i--)
    {
        map[FinalPath->at(i).i][FinalPath->at(i).j].state=2;
        if(FinalPath->at(i).i==X&&FinalPath->at(i).j==Y)//当老鼠经过寻找迷宫路径时，使得state=1，即消除黄色方块
        {
             map[FinalPath->at(i).i][FinalPath->at(i).j].state=1;
            FinalPath->pop_back();
        }
    }
    //画人物，墙壁，终点
    if(!first) //first==true时就要重绘窗口
    {
        for(int i=0;i<M;i++)
        {
            for(int j=0;j<N;j++)
            {
                if(i==X&&j==Y)//如果当前位置时海绵的位置，那就画海绵
                    painter.drawPixmap(j*20,i*20,20,20,QPixmap(":maze/image/haimianbaobao.jpg"));
                else if(i==M-2&&j==N-2)//如果是终点
                    painter.drawPixmap(j*20,i*20,20,20,QPixmap(":maze/image/paidaxing.jpg"));
                else if(i==P&&j==Q)//画章鱼哥的位置
                    painter.drawPixmap(j*20,i*20,20,20,QPixmap(":maze/image/zhangyuge.jpg"));
                else //分三种情况:0:墙壁；1：道路：2：路径
                {
                    switch(map[i][j].state)
                    {
                    case 0://墙壁
                        //用画刷填充整个矩形
//                        painter.setBrush(QBrush(QColor(255,228,225),Qt::SolidPattern));

//                        painter.drawRect(QRect(j*20,i*20,20,20));
                          painter.drawPixmap(j*20,i*20,20,20,QPixmap(":maze/image/wall.jpg"));
                        break;
                    case 1://画道路
                      {
//                        painter.setBrush(QBrush(QColor(118,238,0),Qt::SolidPattern));
//                        painter.drawRect(QRect(j*20,i*20,20,20));
//                          if(!isHasZhangyu)
//                          {
//                           painter.drawPixmap(j*20,i*20,20,20,QPixmap(":maze/image/zhangyuge.jpg"));

//                          }
//                          else
                            painter.drawPixmap(j*20,i*20,20,20,QPixmap(":maze/image/road.jpg"));
                        break;
                      }
                    case 2://画路径
                        painter.setBrush(QBrush(QColor(250,128,114),Qt::SolidPattern));
                        painter.drawRect(QRect(j*20,i*20,20,20));
                        break;
                    }
                }

            }
        }
    }
    //写自动寻路找到路后把路标记出来
//  for(int i=FinalPath->size()-1;i>=0;i--)
//          map[FinalPath->at(i).i][FinalPath->at(i).j].state=2;;//标记为路径

}
MazeWidget::~MazeWidget()
{
    delete ui;
}
//创建迷宫节点矩阵,迷宫的所有节点都是M*N的矩阵里的一个点
void MazeWidget::CreatMaze()
{

    //先设置一个初始节点，因为它可以向上右下左四个方向生成新结点，所以初始点不要设置在(1,1),(2,2)的位置
    int i=3,j=3;
    map[i][j].state=1;

    //设置方向,方向是随机的:0:上；1：右；2：下；3：左
    int randDir=0;
    //设施一个临时节点，它代表了道路
    Maze temp;
    temp.i=i;
    temp.j=j;
    temp.state=1;
    //判断各自的方向上有没有节点,false的话可以创建新节点
    bool Up=false;
    bool Down=false;
    bool Right=false;
    bool Left=false;


    while(1)//不断生成迷宫节点，直至栈为空
    {
        temp.i=i;
        temp.j=j;
        randDir=qrand()%4;
        switch(randDir)
        {
        case 0:
            //要先判断有没有超过边界和上边有没有节点
            if(!Up&&i>2&&map[i-2][j].state==0)
            {
                MazeStack->push(temp);//存入一个节点
                map[i-2][j].state=1;
                map[i-1][j].state=1;
                i=i-2;
                //伪随机生成没有通路的迷宫
                int rand;
                rand=qrand()%10;
                if(rand>3)
                {
                Up=false;
                Right=false;
                Down=false;
                Left=false;
                }
                else
                {
                    Up=true;
                    Right=true;
                    Down=true;
                    Left=true;
                }
            }
            else
                Up=true;//true就不能往上创建新节点了
            break;
          case 1:
            if(!Right &&j<N-3&&map[i][j+2].state==0)
            {
                MazeStack->push(temp);
                map[i][j+2].state=1;
                map[i][j+1].state=1;
                j=j+2;
                int rand;
                rand=qrand()%10;
                if(rand>3)
                {
                Up=false;
                Right=false;
                Down=false;
                Left=false;
                }
                else
                {
                    Up=true;
                    Right=false;
                    Down=true;
                    Left=false;
                }
            }
            else
                Right=true;
            break;
          case 2:
            if(!Down &&i<M-3&&map[i+2][j].state==0)
            {
                MazeStack->push(temp);
                map[i+2][j].state=1;
                map[i+1][j].state=1;
                i=i+2;
                int rand;
                rand=qrand()%10;
                if(rand>3)
                {
                Up=false;
                Right=false;
                Down=false;
                Left=false;
                }
                else
                {
                    Up=true;
                    Right=true;
                    Down=true;
                    Left=true;
                }
            }
            else
                Down=true;
            break;
        case 3:
            if(!Left&&j>2&&map[i][j-2].state==0)
            {
                MazeStack->push(temp);
                map[i][j-2].state=1;
                map[i][j-1].state=1;
                j=j-2;
                Up=false;
                Right=false;
                Down=false;
                Left=false;
            }
            else
                Left=true;
            break;
        }
        //如果四个方向都不能创建新节点了那该怎么办?答案是采用回溯法,弹出栈顶元素，回退到上一个节点再进行判断
        if(Up&&Down&&Right&&Left)
        {
            //先要判断是不是空的
            if(!MazeStack->empty())
            {
                i=MazeStack->top().i;
                j=MazeStack->top().j;
                MazeStack->pop();
                Up=false;
                Down=false;
                Right=false;
                Left=false;
            }
            else//栈为空的话就是迷宫已经创建完成了，即道路已经铺完了
                return ;
        }
    }
}
//有了创建迷宫的函数就可以写槽函数了
void MazeWidget::creatmazebtnSlot()
{
//    isHasZhangyu=false;
    if(isJishu())
    {
        if(Xline->text().toInt()>5&&Yline->text().toInt()>5)
        {
            //重绘图,则需要把原来的map的每个点都删除
            for(int i=0;i<M;i++)
                delete[]map[i];
            delete []map;
            //然后调一下各类控件的位置，都是根据输入的M,N所确定的
            M=Xline->text().toInt();
            N=Yline->text().toInt();
            //重新调整窗口大小
             this->resize((N+10)*20,(M+5)*20);
            Xline->setGeometry(30,(M+2)*20,40,20);
            Yline->setGeometry(90,(M+2)*20,40,20);
            creatmazebtn->setGeometry(150,(M+2)*20,120,30);
          //
            findshortwaybtn->setGeometry((N+2)*20,30,100,30);
            findshortwaybtn->setEnabled(true);
            tips->setGeometry(330,(M+2)*20,190,30);

            recounts->setGeometry((N+2)*20,90,120,50);

            time->setGeometry((N+2)*20,150,120,20);
            time->setStyleSheet("QLabel{background-color:rgb(200,101,202);font-size:25px;color:white;}");
            toScore->setGeometry((N+2)*20,180,120,20);
            toScore->setText(QString("得分:%1").arg(score*k));
            hasPath->setGeometry((N+2)*20,220,120,30);
            hasPath->setEnabled(true);
            FinalPath->clear();//清除原有路径
            this->setFocus(Qt::MouseFocusReason);
            X=0;
            Y=0;
            randpos=qrand()%25;
            if(randpos%2==0)
                randpos++;
            P=randpos;
            Q=randpos;
            //确保人物出生在通道上
            if(X%2==0)
                X+=1;
            if(Y%2==0)
                Y+=1;
            if(P%2==0)
                P+=1;
            if(Q%2==0)
                Q+=1;
           map=new Maze*[M];
           for(int i=0;i<M;i++)
               map[i]=new Maze[N];
           for(int i=0;i<M;i++)
           {
               for(int j=0;j<N;j++)
               {
                   map[i][j].state=0;
                   map[i][j].i=i;
                   map[i][j].j=j;
               }
           }
           //把这些初始化的工作做完就可以调用创建迷宫的函数了
          this->CreatMaze();
           first=false;
           update();//更新

        }
        else
        {
           QMessageBox::information(this,"错误提示","输入的数大于5",QMessageBox::Yes);
           Xline->clear();
           Yline->clear();

        }
    }
    else
    {
        QMessageBox::information(this,"警告","请输入奇数",QMessageBox::Yes);
        Xline->clear();
        Yline->clear();
    }
    //启动定时器
    this->killTimer(timerId);
    timerId=this->startTimer(1000);//每1000ms启动
}
//再写键盘事件，wasd控制人物移动
void MazeWidget::keyPressEvent(QKeyEvent *e)
{
    //人物移动就是重绘地图
    int temp;
    if(e->key()==Qt::Key_W) //上移
    {
        temp=X-1;
        //移动前要判断上面的几点是不是格子或者有没有超过边界
        if(temp &&map[temp][Y].state)
            X=temp;
    }
    else if(e->key()==Qt::Key_S)
    {
        temp=X+1;
        if(temp<M-1 && map[temp][Y].state)
            X=temp;
    }
    else if(e->key()==Qt::Key_A)
    {
        temp=Y-1;
        if(temp &&map[X][temp].state)
            Y=temp;
    }
    else if(e->key()==Qt::Key_D)
    {
        temp=Y+1;
        if(temp<N-1&&map[X][temp].state)
            Y=temp;
    }
    //人物走到了终点,提出对话框，重新自动创建一个等大小的新迷宫
    if(X==P&&Y==Q)//发现章鱼哥彩蛋
    {
           QMessageBox::information(this,"章鱼哥","既然你找到我了，那就给你点奖励(本关通过后积分翻倍)!",QMessageBox::Yes);
           k=2;
    }
    if(X==M-2 && Y==N-2)
    {

        QMessageBox::information(this,"派大星","哦，海绵宝宝你找到我了!",QMessageBox::Yes);
        if(k==1)
        toScore->setText(QString("得分:%1").arg(++score));
        else
        {
            score++;
         toScore->setText(QString("得分:%1").arg(score*2));
         k=1;
        }
//        this->timerEvent();
//        time->setText(QString("%1").arg(300));
        for(int i=0;i<M;i++)
            for(int j=0;j<N;j++)
                map[i][j].state=0;//重新初始化
        CreatMaze();
        FinalPath->clear();
        this->counttimeSlot();
        int rand=0;
        rand=qrand()%2;
        if(rand==0)
        {
            X=3;
            Y=3;
        }
        else
        {
            X=3;
            Y=N-2;
        }
        if(Y%2==0)
            Y+=1;

    }
      update();

}
//再用BFS寻找最短路径
void MazeWidget::FindShortWay()
{
    //当换迷宫时先把上一个迷宫所保存的最短路径从队列里弹出
    //把地图上的最短路径的颜色去除
    FinalPath->clear();
    while(!Box1Queue->empty())
    {
         Box1Queue->pop();
    }
    //创建一个新的二维数组，把原数组的信息复制到里面，因为寻找最短路径时要修改节点信息，
    //所以不能在原数组上修改
    Maze maze[M][N];
    int i,j;
    for( i=0;i<M;i++)
        for( j=0;j<N;j++)
        {
            maze[i][j].i=i;
            maze[i][j].j=j;
            maze[i][j].state=map[i][j].state;
        }
    int t=0,pos=0;
    Box1 box;//先将起点放入队列中
    box.i=X;
    box.j=Y;
    box.pre=-1;
    box.num=t;
    Box1Queue->push(box);
    maze[X][Y].state=-1;//-1代表已经走过该节点,1是还未走过
    Maze temp;
    find=false;
    flag=false;
    while(!Box1Queue->empty()&&!find)
    {
        //把队首的节点入栈
        i=Box1Queue->front().i;
        j=Box1Queue->front().j;
        box.i=i;
        box.j=j;
        box.num=Box1Queue->front().num;
        box.pre=Box1Queue->front().pre;
        Box1Stack->push(box);
        //如果找到了终点，就是按栈原路返回把节点的信息都存入FinalPath里
        if(i==M-2&&j==N-2)
        {
            find=true;
            flag=true;
            //用回溯的方法
            while(!Box1Stack->empty())
            {
                temp.i=Box1Stack->top().i;
                temp.j=Box1Stack->top().j;
                temp.state=1;
                FinalPath->push_back(temp);
                pos=Box1Stack->top().pre;//记录当前节点的上一个节点的坐标的下标
                while(!Box1Stack->empty()&&pos!=Box1Stack->top().num)
                    Box1Stack->pop();

            }

        }
        //尝试4个方向
        int dire=0; //0:上；1：右；2：下；3：左
        for(dire=0;dire<4;dire++)
        {
            switch(dire)
            {
            case 0:
                i=Box1Queue->front().i-1;
                j=Box1Queue->front().j;
                break;
            case 1:
                i=Box1Queue->front().i;
                j=Box1Queue->front().j+1;
                break;
            case 2:
                i=Box1Queue->front().i+1;
                j=Box1Queue->front().j;
                break;
            case 3:
                i=Box1Queue->front().i;
                j=Box1Queue->front().j-1;
                break;
            }
            if(maze[i][j].state==1) //还没走过
            {
                t++;
                box.i=i;
                box.j=j;
                box.num=t;
                box.pre=Box1Queue->front().num;
                Box1Queue->push(box);
                maze[i][j].state=-1;
            }
        }
         Box1Queue->pop();
    }

}
//写寻找最短路径按钮的槽函数
void MazeWidget::findshortwaybtnSlot()
{
    for(int i=0;i<M;i++)
        for(int j=0;j<N;j++)
        {
            if(map[i][j].state==2)
                map[i][j].state=1;
        }
    FindShortWay();
    update();

}
//还要再添加计时器
void MazeWidget::timerEvent(QTimerEvent *)
{
   counttimeSlot();
}
void MazeWidget::counttimeSlot()
{
   static int  sec=300;
    time->setText(QString("%1").arg(sec--));
    if(time->text().toInt()==0)
    {
        this->killTimer(timerId);
       int ret= QMessageBox::information(this,"请重新开始游戏","Game Over!",QMessageBox::Yes);
       if(ret==QMessageBox::Yes)
          this->creatmazebtnSlot();
        //点击重新开始游戏
     sec=300;
     score=0;
     toScore->setText(QString("得分:%1").arg(score));
    }
}

void MazeWidget::isHasPathSlot()
{

    this->findshortwaybtnSlot();
    if(flag==false)
    {

        QMessageBox::information(this,"没有通路","对的，没有通路，得一分",QMessageBox::Yes);
        this->creatmazebtnSlot();
        toScore->setText(QString("得分:%1").arg(++score));
    }
    else
    {
        QMessageBox:: QMessageBox::information(this,"有通路","有通路,分数清0",QMessageBox::Yes);
        score=0;
        toScore->setText(QString("得分:%1").arg(score*k));
    }
}
