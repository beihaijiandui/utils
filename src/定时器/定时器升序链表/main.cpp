#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <pthread.h>
#include "ascending_timer_linked_list.h"

#define FD_LIMIT 65535
#define MAX_EVENT_NUMBER 1024
#define TIMESLOT 5

static int pipefd[2];
//利用头文件中的升序链表管理定时器
static sort_timer_lst timer_lst;
static int epollfd = 0;


int main(int argc, char* argv[])
{
	if (argc <= 2)
	{
		printf("usage:%s ip_address port_number\n",basename(argv[0]));
		return 1;
	}

}