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
            util_timer* tmp = head;
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
            //如果目标定时器的超时时间小于当前链表中所有定时器的超时时间，则把该定时器
            //插入链表头部，作为链表新的头结点。否则就要调用重载函数
            //add_timer(util_timer* timer, util_timer* lst_head),
            //把它插入到链表中的合适的位置，以保证链表的升序特性。
            if(timer->expire < head->expire)
            {
                timer->next = head;
                head->prev = timer;
                head = timer;
                return;
            }
            add_timer(timer,head);
        }

        //当某个定时任务发生变化时，调整对应的定时器在链表中的位置。这个函数只考虑被调整的定时器的超时时间延长的情况，即
        //该定时器需要往链表的尾部移动
        void adjust_timer(util_timer* timer)
        {
            if (!timer)
            {
                return;
            }
            util_timer* tmp = timer->next;
            //如果被调整的目标定时器处在链表尾部，或者该定时器新的超时值仍然小于其下一个定时器的超时值，
            //则不用调整
            if (!tmp || (timer->expire < tmp->expire))
            {
                return;
            }
            //如果目标定时器是链表的头节点，则将该定时器从链表中取出并重新插入链表
            if (timer == head)
            {
                head = head->next;
                head->prev = nullptr;
                timer->next = nullptr;   //因为是先查询到的链表中的timer，要断开本来的前后指向的连接
                add_timer(timer, head);
            }
            else                        //如果目标定时器不是链表的头结点，则将该定时器从链表中取出，然后插入其原来所在位置之后的
            {                           //部分链表中
                timer->prev->next = timer->next;
                timer->next->prev = timer->prev;
                add_timer(timer, timer->next);
            }
        }

        //将目标定时器timer从链表中删除
        void del_timer(util_timer* timer)
        {
            if(!timer)
            {
                return;
            }
            //链表中只有一个定时器，即是目标定时器
            if((head==timer)&&(tail==timer))
            {
                delete timer;
                head=nullptr;
                tail=nullptr;
                return;
            }
            //如果链表中至少有2个定时器，且目标定时器是链表的头结点，则将链表的头结点重置为原头结点的下一个节点
            //然后删除目标定时器
            if(timer == head)
            {
                head= head->next;
                head->prev=nullptr;
                delete timer;
                return;
            }
            //如果链表中至少有2个定时器，且目标定时器是链表的尾结点，则将链表的尾结点重置为原尾结点的前一个节点
            //然后删除目标定时器
            if(timer==tail)
            {
                tail = tail->prev;
                tail->next = nullptr;
                delete timer;
                return;
            }
            //如果目标定时器位于链表的中间，则把他前后的定时器串联起来，然后删除目标定时器
            timer->prev->next = timer->next;
            timer->next->prev = timer->prev;
            delete timer;
            return;
        }
    private:
        //一个重载的辅助函数，他被公有的add_timer函数和adjust_timer函数调用，该函数表示将目标定时器timer
        //添加到lst_head之后的公共链表中
        void add_timer(util_timer* timer, util_timer* lst_head)
        {
            util_timer* prev = lst_head;
            util_timer* tmp = prev->next;
            //遍历lst_head节点之后的部分链表，直到找到一个超时时间大于目标定时器的超时时间的节点，并将目标
            //定时器插入该节点之前
            while (tmp)
            {
                if (timer->expire < tmp->expire)
                {
                    prev->next = timer;
                    timer->next = tmp; 
                    tmp->prev = timer;
                    timer->prev = prev;
                    break;
                }
                prev = tmp;
                tmp = tmp->next;
            }
            //
            if (!tmp)
            {
                prev->next = timer;
                timer->next = nullptr;
                timer->prev = prev;
                tail = timer;
            }
        }
    private:
        //链表本身不需要考虑前指后指，有头尾节点即可
        util_timer* head;
        util_timer* tail;
};

#endif 
