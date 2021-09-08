#include "global.h"


extern pthread_cond_t cond_M0; //M0控制命令发送线程被唤醒条件变量
extern pthread_mutex_t mutex_M0;  //M0控制命令发送线程互斥锁
struct msg global_msg;


void *pthread_M0_control(void *arg)
{
	int s;
	while(1)
	{
		pthread_mutex_lock(&mutex_M0);
		pthread_cond_wait(&cond_M0,&mutex_M0);
		printf("type = %ld\ntext = %d\n",global_msg.msgtype,global_msg.text[0]);
		if((s = write(dev_uart_fd,&global_msg,sizeof(global_msg))) < 0)
		{
			ERR_MSG("write");
			return NULL;
		}

printf("__%s__  ,  __%d__ s = %d\n",__func__,__LINE__,s);
		pthread_mutex_unlock(&mutex_M0);
	}

	pthread_exit(NULL);
}


