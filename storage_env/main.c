#include "global.h"



/* 初始化互斥锁和条件变量*/
pthread_cond_t         cond_sqlite = PTHREAD_COND_INITIALIZER;   //数据库线程被唤醒条件变量
pthread_cond_t       cond_analysis = PTHREAD_COND_INITIALIZER;   //数据分析线程被唤醒条件变量
pthread_cond_t       cond_uart_cmd = PTHREAD_COND_INITIALIZER;   //M0控制命令发送线程被唤醒条件变量
pthread_cond_t cond_client_requset = PTHREAD_COND_INITIALIZER;   //处理消息队列里请求的线程被唤醒条件变量
pthread_cond_t       cond_infrared = PTHREAD_COND_INITIALIZER;   //红外监测线程被唤醒条件变量
pthread_cond_t         cond_buzzer = PTHREAD_COND_INITIALIZER;   //A7蜂鸣器控制线程被唤醒条件变量
pthread_cond_t            cond_led = PTHREAD_COND_INITIALIZER;   //A7LED模块线程被唤醒条件变量
pthread_cond_t         cond_camera = PTHREAD_COND_INITIALIZER;   //摄像头模块控制线程被唤醒条件
pthread_cond_t            cond_sms = PTHREAD_COND_INITIALIZER;   //短信模块控制线程被唤醒条件变量
pthread_cond_t        cond_refresh = PTHREAD_COND_INITIALIZER;   //更新共享内存里的实时数据线程被唤醒条件变量
pthread_cond_t             cond_M0 = PTHREAD_COND_INITIALIZER;   //更新共享内存里的实时数据线程被唤醒条件变量

pthread_mutex_t 		mutex_slinklist = PTHREAD_MUTEX_INITIALIZER;  //数据库缓存互斥锁
pthread_mutex_t 		   mutex_sqlite = PTHREAD_MUTEX_INITIALIZER;  //数据库线程互斥锁
pthread_mutex_t 		 mutex_analysis = PTHREAD_MUTEX_INITIALIZER;  //数据分析线程互斥锁
pthread_mutex_t 		 mutex_uart_cmd = PTHREAD_MUTEX_INITIALIZER;  //M0控制命令发送线程互斥锁
pthread_mutex_t mutex_client_request = PTHREAD_MUTEX_INITIALIZER;  //处理消息队列里的请求的线程互斥锁
pthread_mutex_t       mutex_infrared = PTHREAD_MUTEX_INITIALIZER;  //红外监测线程互斥锁 
pthread_mutex_t         mutex_buzzer = PTHREAD_MUTEX_INITIALIZER;  //A7蜂鸣器控制线程互斥锁
pthread_mutex_t            mutex_led = PTHREAD_MUTEX_INITIALIZER;   //A7LED模块线程互斥锁
pthread_mutex_t         mutex_camera = PTHREAD_MUTEX_INITIALIZER;  //摄像头模块控制线程互斥锁
pthread_mutex_t            mutex_sms = PTHREAD_MUTEX_INITIALIZER;   //短信模块控制线程互斥锁
pthread_mutex_t        mutex_refresh = PTHREAD_MUTEX_INITIALIZER;  //更新共享内存里的实时数据线程互斥锁
pthread_mutex_t mutex_refresh_updata = PTHREAD_MUTEX_INITIALIZER;  //更新共享内存里的实时数据互斥锁
pthread_mutex_t         mutex_global = PTHREAD_MUTEX_INITIALIZER;   //全局变量保护互斥锁
pthread_mutex_t 	  mutex_linklist = PTHREAD_MUTEX_INITIALIZER;  //接受数据缓存互斥锁
pthread_mutex_t 	        mutex_M0 = PTHREAD_MUTEX_INITIALIZER;  //接受数据缓存互斥锁

pthread_t tid_client_request,
		  tid_refresh, 
		  tid_sqlite, 
		  tid_transfer, 
		  tid_analysis, 
		  tid_buzzer, 
		  tid_led, 
		  tid_camera, 
		  tid_sms,
		  tid_M0_control;


int create_pthread(void);
void free_resources(void);

int main(int argc, const char *argv[])
{
	//创建各模块线程
/*
	if(create_pthread() != 0);	
	{
		ERR_MSG("pthread_create");
		return -1;
	}
	*/
if(pthread_create(&tid_client_request, NULL,pthread_client_request, NULL) != 0)
	{
		ERR_MSG("pthread_create tid_client_request");
		return -1;
	}

	if(pthread_create(&tid_refresh, NULL, pthread_refresh, NULL) != 0)
	{
		ERR_MSG("pthread_create tid_refresh");
		return -1;
	}

	if(pthread_create(&tid_M0_control, NULL, pthread_M0_control, NULL) != 0)
	{
		ERR_MSG("pthread_create tid_M0_control");
		return -1;
	}


	if(pthread_create(&tid_sqlite, NULL, pthread_sqlite, NULL) != 0)
	{
		ERR_MSG("pthread_create tid_sqlite");
		return -1;
	}

	if(pthread_create(&tid_transfer, NULL, pthread_transfer, NULL) != 0)
	{
		ERR_MSG("pthread_create tid_transfer");
		return -1;
	}

	if(pthread_create(&tid_analysis, NULL, pthread_analysis, NULL) != 0)
	{
		ERR_MSG("pthread_create tid_analysis");
		return -1;
	}

	if(pthread_create(&tid_buzzer, NULL, pthread_buzzer, NULL) != 0)
	{
		ERR_MSG("pthread_create tid_buzzer");
		return -1;
	}

	if(pthread_create(&tid_led, NULL, pthread_led, NULL) != 0)
	{
		ERR_MSG("pthread_create tid_led");
		return -1;
	}

	if(pthread_create(&tid_camera, NULL, pthread_camera, NULL) != 0)
	{
	ERR_MSG("pthread_create tid_camera");
	return -1;
	}



	free_resources();

	exit(0);
}

int create_pthread(void)
{
	if(pthread_create(&tid_client_request, NULL,pthread_client_request, NULL) != 0)
	{
		ERR_MSG("pthread_create tid_client_request");
		return -1;
	}

	if(pthread_create(&tid_refresh, NULL, pthread_refresh, NULL) != 0)
	{
		ERR_MSG("pthread_create tid_refresh");
		return -1;
	}

	if(pthread_create(&tid_M0_control, NULL, pthread_M0_control, NULL) != 0)
	{
		ERR_MSG("pthread_create tid_M0_control");
		return -1;
	}

	if(pthread_create(&tid_sqlite, NULL, pthread_sqlite, NULL) != 0)
	{
		ERR_MSG("pthread_create tid_sqlite");
		return -1;
	}

	if(pthread_create(&tid_transfer, NULL, pthread_transfer, NULL) != 0)
	{
		ERR_MSG("pthread_create tid_transfer");
		return -1;
	}

	if(pthread_create(&tid_analysis, NULL, pthread_analysis, NULL) != 0)
	{
		ERR_MSG("pthread_create tid_analysis");
		return -1;
	}

	if(pthread_create(&tid_buzzer, NULL, pthread_buzzer, NULL) != 0)
	{
		ERR_MSG("pthread_create tid_buzzer");
		return -1;
	}

	if(pthread_create(&tid_led, NULL, pthread_led, NULL) != 0)
	{
		ERR_MSG("pthread_create tid_led");
		return -1;
	}

	if(pthread_create(&tid_camera, NULL, pthread_camera, NULL) != 0)
	{
	ERR_MSG("pthread_create tid_camera");
	return -1;
	}
 	return 0;
}

void free_resources(void)
{
	pthread_join(tid_client_request, NULL);
	pthread_join(tid_refresh, NULL);
	pthread_join(tid_sqlite, NULL);
	pthread_join(tid_transfer, NULL);
	pthread_join(tid_analysis, NULL);
	pthread_join(tid_buzzer, NULL);
	pthread_join(tid_led, NULL);
	pthread_join(tid_camera, NULL);

	pthread_mutex_destroy(&mutex_slinklist);
	pthread_mutex_destroy(&mutex_sqlite);
	pthread_mutex_destroy(&mutex_analysis);
	pthread_mutex_destroy(&mutex_uart_cmd);
	pthread_mutex_destroy(&mutex_client_request);
	pthread_mutex_destroy(&mutex_infrared);
	pthread_mutex_destroy(&mutex_buzzer);
	pthread_mutex_destroy(&mutex_led);
	pthread_mutex_destroy(&mutex_camera);
	pthread_mutex_destroy(&mutex_sms);
	pthread_mutex_destroy(&mutex_refresh);
	pthread_mutex_destroy(&mutex_refresh_updata);
	pthread_mutex_destroy(&mutex_global);
	pthread_mutex_destroy(&mutex_linklist);
	pthread_mutex_destroy(&mutex_M0);

	pthread_cond_destroy(&cond_sqlite);
	pthread_cond_destroy(&cond_analysis);
	pthread_cond_destroy(&cond_uart_cmd);
	pthread_cond_destroy(&cond_client_requset);
	pthread_cond_destroy(&cond_infrared);
	pthread_cond_destroy(&cond_buzzer);
	pthread_cond_destroy(&cond_led);
	pthread_cond_destroy(&cond_camera);
	pthread_cond_destroy(&cond_sms);
	pthread_cond_destroy(&cond_refresh);
	pthread_cond_destroy(&cond_M0);

}
