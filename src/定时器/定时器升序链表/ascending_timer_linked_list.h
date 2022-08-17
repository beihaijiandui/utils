#ifndef LST_TIMER
#define LST_TIMER

#include <time.h>

#define BUFFER_SIZE 64
class util_timer;     //ǰ������

//�û����ݽṹ
//�ͻ���socket��ַ��socket�ļ���������������Ͷ�ʱ��
struct client_data
{
    sockaddr_in address;
    int sockfd;
    char buf[BUFFER_SIZE];
    util_timer* timer;
};

//��ʱ����
class util_timer
{
    public:
        util_timer():prev(nullptr),next(nullptr){}
    public:
        time_t expire;                   //����ĳ�ʱʱ�䣬����ʹ�þ���ʱ��
        void ��*cb_func��(client_data*); //����ص�����
        client_data* user_data;         //�ص���������Ŀͻ����ݣ��ɶ�ʱ����ִ���ߴ��ݸ��ص�����
        util_timer* prev;               //ָ��ǰһ����ʱ��
        util_timer* next;               //ָ����һ����ʱ��
};

//��ʱ����������˫��������ͷ����β���
class sort_timer_lst
{
    public:
        sort_timer_lst():head(nullptr),tail(nullptr){}
        //��������ʱ��ɾ���������еĶ�ʱ��
        ~sort_timer_lst()
        {
            util_timer* tmp = head;
            while (tmp)      //tmp�ǿ�ʱ���ſ���ȡtmpָ���ָ������
            {
                head = tmp->next;
                delete tmp;
                tmp = head;
            }
        }

        //��Ŀ�궨ʱ��timer��ӵ�������
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
            //���Ŀ�궨ʱ���ĳ�ʱʱ��С�ڵ�ǰ���������ж�ʱ���ĳ�ʱʱ�䣬��Ѹö�ʱ��
            //��������ͷ������Ϊ�����µ�ͷ��㡣�����Ҫ�������غ���
            //add_timer(util_timer* timer, util_timer* lst_head),
            //�������뵽�����еĺ��ʵ�λ�ã��Ա�֤������������ԡ�
            if(timer->expire < head->expire)
            {
                timer->next = head;
                head->prev = timer;
                head = timer;
                return;
            }
            add_timer(timer,head);
        }

        //��ĳ����ʱ�������仯ʱ��������Ӧ�Ķ�ʱ���������е�λ�á��������ֻ���Ǳ������Ķ�ʱ���ĳ�ʱʱ���ӳ����������
        //�ö�ʱ����Ҫ�������β���ƶ�
        void adjust_timer(util_timer* timer)
        {
            if (!timer)
            {
                return;
            }
            util_timer* tmp = timer->next;
            //�����������Ŀ�궨ʱ����������β�������߸ö�ʱ���µĳ�ʱֵ��ȻС������һ����ʱ���ĳ�ʱֵ��
            //���õ���
            if (!tmp || (timer->expire < tmp->expire))
            {
                return;
            }
            //���Ŀ�궨ʱ���������ͷ�ڵ㣬�򽫸ö�ʱ����������ȡ�������²�������
            if (timer == head)
            {
                head = head->next;
                head->prev = nullptr;
                timer->next = nullptr;   //��Ϊ���Ȳ�ѯ���������е�timer��Ҫ�Ͽ�������ǰ��ָ�������
                add_timer(timer, head);
            }
            else                        //���Ŀ�궨ʱ�����������ͷ��㣬�򽫸ö�ʱ����������ȡ����Ȼ�������ԭ������λ��֮���
            {                           //����������
                timer->prev->next = timer->next;
                timer->next->prev = timer->prev;
                add_timer(timer, timer->next);
            }
        }

        //��Ŀ�궨ʱ��timer��������ɾ��
        void del_timer(util_timer* timer)
        {
            if(!timer)
            {
                return;
            }
            //������ֻ��һ����ʱ��������Ŀ�궨ʱ��
            if((head==timer)&&(tail==timer))
            {
                delete timer;
                head=nullptr;
                tail=nullptr;
                return;
            }
            //���������������2����ʱ������Ŀ�궨ʱ���������ͷ��㣬�������ͷ�������Ϊԭͷ������һ���ڵ�
            //Ȼ��ɾ��Ŀ�궨ʱ��
            if(timer == head)
            {
                head= head->next;
                head->prev=nullptr;
                delete timer;
                return;
            }
            //���������������2����ʱ������Ŀ�궨ʱ���������β��㣬�������β�������Ϊԭβ����ǰһ���ڵ�
            //Ȼ��ɾ��Ŀ�궨ʱ��
            if(timer==tail)
            {
                tail = tail->prev;
                tail->next = nullptr;
                delete timer;
                return;
            }
            //���Ŀ�궨ʱ��λ��������м䣬�����ǰ��Ķ�ʱ������������Ȼ��ɾ��Ŀ�궨ʱ��
            timer->prev->next = timer->next;
            timer->next->prev = timer->prev;
            delete timer;
            return;
        }

        //SIGALRM�ź�ÿ�α������������źŴ����������ʹ��ͳһ�¼�Դ����������������ִ��һ��tick�������Դ��������ϵ��ڵ�����
        void tick()
        {
            if (!head)
            {
                return;
            }
            printf("timer tick\n");
            //��ȡϵͳ��ǰʱ��
            time_t cur = time(NULL);
            util_timer* tmp = head;
            //��ͷ��㿪ʼ���δ���ÿ����ʱ����ֱ������һ����δ���ڵĶ�ʱ��������Ƕ�ʱ���ĺ����߼�
            while (tmp)
            {
                //��Ϊÿ����ʱ����ʹ�þ���ʱ����Ϊ��ʱֵ���������ǿ��԰Ѷ�ʱ���ĳ�ʱֵ��ϵͳ��ǰʱ��Ƚ����ж϶�ʱ���Ƿ���
                if (cur < tmp->expire)
                {
                    break;
                }
                tmp->cb_func(tmp->user_data);
                //ִ���궨ʱ���еĶ�ʱ����֮�󣬾ͽ�����������ɾ��������������ͷ���
                head = tmp->next;
                if (head)
                {
                    head->prev = nullptr;
                }
                delete tmp;
                tmp = head;           //Ҫô�������ڣ�Ҫô��ֹһ�����ڣ�����Ҫwhile�����һ�£�ÿ�ε���һ��tick����һ���Ų���ڶ�ʱ����Ѱ��
            }
        }
    private:
        //һ�����صĸ����������������е�add_timer������adjust_timer�������ã��ú�����ʾ��Ŀ�궨ʱ��timer
        //��ӵ�lst_head֮��Ĺ���������
        void add_timer(util_timer* timer, util_timer* lst_head)
        {
            util_timer* prev = lst_head;
            util_timer* tmp = prev->next;
            //����lst_head�ڵ�֮��Ĳ�������ֱ���ҵ�һ����ʱʱ�����Ŀ�궨ʱ���ĳ�ʱʱ��Ľڵ㣬����Ŀ��
            //��ʱ������ýڵ�֮ǰ
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
        //��������Ҫ����ǰָ��ָ����ͷβ�ڵ㼴��
        util_timer* head;
        util_timer* tail;
};

#endif 
