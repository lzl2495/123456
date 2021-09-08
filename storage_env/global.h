#ifndef __GLOBAL_H__
#define __GLOBAL_H__
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <pthread.h>
#include <sys/msg.h>
//#include "link_list.h"

#define ERR_MSG(msg) do{ \
	printf("__%d__,%s\n",__LINE__,__func__);\
	perror(msg);\
}while(0);


#define QUEUE_MSG_LEN 128
#define DEV_ZIGBEE "/dev/ttyUSB0"
#define STO_NO 1

//仓库信息结构体
struct storage_env_info
{
	unsigned char storage_status;   //仓库开关状态
	unsigned char led_status;       //仓库LED状态
	unsigned char buzzer_status;    //仓库Buzzer状态
	unsigned char fan_status;       //仓库风扇状态
	unsigned char seg_status;       //仓库数码管状态
	signed char x;                  //仓库采集端三轴状态
	signed char y;                  //
	signed char z;                  //
	float temperature;              //仓库当前温度
	float temperatureMIN;           //仓库温度上限
	float temperatureMAX;           //仓库温度下限
	float humidity;					//仓库当前湿度
	float humidityMIN;              //仓库仓库湿度上限
	float humidityMAX;                //仓库湿度下限
	float illumination;				//光照强度
	float illuminationMIN;            //仓库湿度上限
	float illuminationMAX;         //仓库光照下限
	float battery;			  		//仓库采集端电池电压
	float adc;                  //仓库adc采集电压

};

//环境信息结构体数据包，从M0获取
struct env_info_package
{
	unsigned char sto_no;
	unsigned char tem[2];   //温度信息
	unsigned char hum[2];   //湿度信息
	unsigned char x;        //仓库采集端三轴状态
	unsigned char y;
	unsigned char z;
	unsigned int ill;       //光照强度
	unsigned int battery;   //仓库采集端电池电压
	unsigned int adc;       //仓库adc采集电压
};

//设置环境信息，用于消息队列
struct setEnv
{
	int temMAX;
	int temMIN;
	int humMAX;
	int humMIN;
	int illMAX;
	int illMIN;
};

//消息队列结构体
struct msg
{
	long msgtype;  // 消息类型
	unsigned int text[QUEUE_MSG_LEN];  //消息正文
};

void *pthread_client_request(void *arg);  //处理消息队列里请求的线程.
void *pthread_refresh(void *arg);   //更新共享内存里的实时数据线程.
void *pthread_sqlite(void *arg);   //数据库线程.
void *pthread_transfer(void *arg);   //接收M0数据线程.
void *pthread_analysis(void *arg);   //M0数据分析线程.
void *pthread_buzzer(void *arg);    //A7蜂鸣器控制线程.
void *pthread_M0_control(void *arg); //控制M0线程
void *pthread_led(void *arg);    //A7LED模块线程.
void *pthread_camera(void *arg);   //摄像头模块控制线程.
//void *pthread_sms(void *arg);   //短信模块控制线程.

//条件变量
extern pthread_cond_t cond_sqlite;    //数据库线程被唤醒条件变量
extern pthread_cond_t cond_analysis;  //数据分析线程被唤醒条件变量
extern pthread_cond_t cond_uart_cmd; //M0控制命令发送线程被唤醒条件变量
extern pthread_cond_t cond_client_requset; //处理消息队列里请求的线程被唤醒条件变量
extern pthread_cond_t cond_infrared;  //红外监测线程被唤醒条件变量
extern pthread_cond_t cond_buzzer;  //A7蜂鸣器控制线程被唤醒条件变量
extern pthread_cond_t cond_led;  //A7LED模块线程被唤醒条件变量
extern pthread_cond_t cond_camera;  //摄像头模块控制线程被唤醒条件
extern pthread_cond_t cond_sms;  //短信模块控制线程被唤醒条件变量
extern pthread_cond_t cond_refresh;  //更新共享内存里的实时数据线程被唤醒条件变量
extern pthread_cond_t cond_M0;  //M0控制命令发送线程被唤醒条件变量


//互斥锁
extern pthread_mutex_t mutex_slinklist;  //数据库缓存互斥锁
extern pthread_mutex_t mutex_sqlite;  //数据库线程互斥锁
extern pthread_mutex_t mutex_analysis;  //数据分析线程互斥锁
extern pthread_mutex_t mutex_uart_cmd;  //M0控制命令发送线程互斥锁
extern pthread_mutex_t mutex_client_request;  //处理消息队列里的请求的线程互斥锁
extern pthread_mutex_t mutex_infrared;  //红外监测线程互斥锁 
extern pthread_mutex_t mutex_buzzer;  //A7蜂鸣器控制线程互斥锁
extern pthread_mutex_t mutex_led;   //A7LED模块线程互斥锁
extern pthread_mutex_t mutex_camera;  //摄像头模块控制线程互斥锁
extern pthread_mutex_t mutex_sms;   //短信模块控制线程互斥锁
extern pthread_mutex_t mutex_refresh;  //更新共享内存里的实时数据线程互斥锁
extern pthread_mutex_t mutex_refresh_updata;  //更新共享内存里的实时数据互斥锁
extern pthread_mutex_t mutex_global;   //全局变量保护互斥锁
extern pthread_mutex_t mutex_linklist;  //接受数据缓存互斥锁
extern pthread_mutex_t mutex_M0;  //M0控制命令发送线程被唤醒条件变量

struct storage_env_info storage_RT;
struct msg global_msg;


int dev_uart_fd;  //M0模块文件描述符
//linklist linkHead;  //M0消息接受链表
//slinklist slinkHead; //处理M0消息的

int msgid;


#endif  /*__GLOBAL_H__*/
