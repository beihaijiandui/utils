#ifndef LST_TIMER
#define LST_TIMER

#include <time.h>

#define BUFFER_SIZE 64
class util_timer;     //前向声明

//用户数据结构
//客户端socket地址、socket文件描述符、读缓存和定时器
struct client_data
{
    sockaddr_in address;
    int sockfd;
    char buf[BUFFER_SIZE];
    util_timer* timer;
};

//定时器类
class util_timer
{
    public:
        util_timer():prev(nullptr),next(nullptr){}
    public:
        time_t expire;                   //任务的超时时间，这里使用绝对时间
        void （*cb_func）(client_data*); //任务回调函数
        client_data* user_data;         //回调函数处理的客户数据，由定时器的执行者传递给回调函数
        util_timer* prev;               //指向前一个定时器
        util_timer* next;               //指向下一个定时器
};

//定时器链表，升序、双向链表，有头结点和尾结点
class sort_timer_lst
{
    public:
        sort_timer_lst():head(nullptr),tail(nullptr){}
        //链表被销毁时，删除其中所有的定时器
        ~sort_timer_lst()
        {
            utill_timer* tmp = head;
            while (tmp)      //tmp非空时，才可以取tmp指向的指针内容
            {
                head = tmp->next;
                delete tmp;
                tmp = head;
            }
        }

        //将目标定时器timer添加到链表中
        void add_timer(util_timer* timer)
        {
            if(!timer)
            {
                return;
            }
            if(!head)
            {
                head = tail = timer;
                return;
            }
            if(timer->expire < head->expire)
            {
                timer->next = head;
                head->prev = timer;
                head = timer;
                return;
            }
            //如果目标定时器的超时时间小于当前链表中所有定时器的超时时间，则把该定时器
            //插入链表头部，作为链表新的头结点。否则就要调用重载函数
            //add_timer(util_timer* timer, util_timer* lst_head),
            //把它插入到链表中的合适的位置，以保证链表的升序特性。
            add_timer(timer,head);
        }
};