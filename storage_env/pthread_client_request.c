#include "global.h"
#include "link_list.h"

#define LED1 1
#define LED1_ON 1
#define LED1_OFF 0
#define LED2 6
#define LED2_ON 1
#define LED2_OFF 0
#define LED3 7
#define LED3_ON 1
#define LED3_OFF 0
#define BEEP 2
#define BEEP_ON 1
#define BEEP_OFF 0
#define SEG 3
#define SEG_OFF 0
#define SEG_0 10
#define SEG_1 1
#define SEG_2 2
#define SEG_3 3
#define SEG_4 4
#define SEG_5 5
#define SEG_6 6
#define SEG_7 7
#define SEG_8 8
#define SEG_9 9
#define FAN 4
#define FAN_LEVEL1 1
#define FAN_LEVEL2 2
#define FAN_LEVEL3 3
#define FAN_OFF 0

extern pthread_cond_t cond_analysis;
extern pthread_cond_t cond_uart_cmd;
extern pthread_mutex_t mutex_linklist;
extern pthread_mutex_t mutex_uart_cmd;
extern unsigned int text_choose[16];
extern struct storage_env_info storage_RT;
extern struct msg global_msg;
extern int msgid;

void *pthread_client_request(void *arg)
{
	//定义消息队列结构体
	struct msg msgsz;
	int i;


	key_t key = ftok("/home",1);
	if(key < 0)
	{
		ERR_MSG("ftok");
		return NULL;
	}

	msgid = msgget(key,IPC_CREAT|0664);
	if(msgid < 0)
	{
		ERR_MSG("msgget");
		return NULL;
	}

	while(1)
	{
		ssize_t size = msgrcv(msgid, &msgsz, sizeof(msgsz.text), 0, 0);
		if(size < 0)
		{
			ERR_MSG("msgrcv");
			return NULL;
		}

		printf("type = %ld\ntext = %d\n",msgsz.msgtype,msgsz.text[0]);
printf("__%s__  ,  __%d__\n",__func__,__LINE__);
		switch(msgsz.msgtype)
		{
		case 1:
			//LED数码管 M0操作
			pthread_mutex_lock (&mutex_uart_cmd);
			//将新数据存入全局变量用来更新数据库数据
printf("__%s__  ,  __%d__\n",__func__,__LINE__);
			global_msg.msgtype = msgsz.msgtype;
			for(i = 0; i < 16; i++)
			{
				global_msg.text[i] = msgsz.text[i];
			}

printf("__%s__  ,  __%d__\n",__func__,__LINE__);
			pthread_mutex_unlock (&mutex_uart_cmd);
printf("__%s__  ,  __%d__\n",__func__,__LINE__);
			pthread_cond_signal (&cond_M0);

			break;
		case 2:
			//Beep M0操作
			pthread_mutex_lock (&mutex_uart_cmd);
			//将新数据存入全局变量用来更新数据库数据
			global_msg.msgtype = msgsz.msgtype;
			for(i = 0; i < 16; i++)
			{
				global_msg.text[i] = msgsz.text[i];
			}

			pthread_mutex_unlock (&mutex_uart_cmd);
			pthread_cond_signal (&cond_M0);
			break;
		case 3:
			//Seg M0操作
			pthread_mutex_lock (&mutex_uart_cmd);
			//将新数据存入全局变量用来更新数据库数据
			global_msg.msgtype = msgsz.msgtype;
			for(i = 0; i < 16; i++)
			{
				global_msg.text[i] = msgsz.text[i];
			}

			pthread_mutex_unlock (&mutex_uart_cmd);
			pthread_cond_signal (&cond_M0);
			break;
		case 4:
			//FAN M0操作
			//
			pthread_mutex_lock (&mutex_uart_cmd);
			//将新数据存入全局变量用来更新数据库数据
			global_msg.msgtype = msgsz.msgtype;
			for(i = 0; i < 16; i++)
			{
				global_msg.text[i] = msgsz.text[i];
			}

			pthread_mutex_unlock (&mutex_uart_cmd);
			pthread_cond_signal (&cond_M0);
			break;
		case 5:
			//数据库最大最小值更新
			printf("__%s__  ,  __%d__\n",__func__,__LINE__);
			pthread_mutex_lock(&mutex_linklist);
			//将新数据存入全局变量用来更新数据库数据
			storage_RT.temperatureMAX 	= (float)msgsz.text[0];
			storage_RT.temperatureMIN 	= (float)msgsz.text[1];
			storage_RT.humidityMAX 		= (float)msgsz.text[2];
			storage_RT.humidityMIN 		= (float)msgsz.text[3];
			storage_RT.illuminationMAX 	= (float)msgsz.text[4];
			storage_RT.illuminationMIN 	= (float)msgsz.text[5];
			printf("__%s__  ,  __%d__\n",__func__,__LINE__);

			link_datatype buf;
			buf.msg_type = 'u';

			printf("__%s__  ,  __%d__\n",__func__,__LINE__);
			InsertLinknode(buf);
			printf("__%s__  ,  __%d__\n",__func__,__LINE__);

			pthread_mutex_unlock (&mutex_linklist);
			printf("__%s__  ,  __%d__\n",__func__,__LINE__);
			pthread_cond_signal (&cond_analysis);
			printf("__%s__  ,  __%d__\n",__func__,__LINE__);
			break;

		}
	}
	return 0;
}
