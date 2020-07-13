#ifndef STACK_H_
#define STACK_H_
//用单向链表实现
template<class T>
class stackNode{
public:
    stackNode():next(nullptr){}
    T data;//当前节点存储的数据
    stackNode*next;//指向下一个节点的指针
};
template<class T>
class Stack{
  private:
    int len;
    stackNode<T>*node;//临时节点
    stackNode<T>*headnode;//头节点
public:
    Stack();
    int size(){return len;}
    void push(T t);
    void pop();
    T top();
    bool empty();
    void clear();
};
template<class T>
Stack<T>::Stack()
{
    node=nullptr;
    headnode=nullptr;
    len=0;
}
template<class T>
void Stack<T>::push(T t)
{
    node=new stackNode<T>();
    node->data=t;
    node->next=headnode;
    headnode=node;
    ++len;

}
template<class T>
void Stack<T>::pop()
{
    if(!empty())
    {
    node=headnode;
    headnode=headnode->next;//头节点变成它的下一个节点
    delete(node);//删除头节点
    --len;
    }
}
template<class T>
bool Stack<T>::empty()
{
    return len==0;
}
template<class T>
T Stack<T>::top()
{
    if(!empty())
        return headnode->data;
}
template<class T>
void Stack<T>::clear()
{
    while(headnode!=nullptr)
    {
        node=headnode;
        headnode=headnode->next;
        delete(node);
    }
    node=nullptr;
    headnode=nullptr;
    len=0;
}
#endif
