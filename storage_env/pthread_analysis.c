#include "global.h"
#include "link_list.h"
#include "sqlite_link_list.h"

extern linklist linkHead;
extern linklist slinkHead;
extern pthread_mutex_t mutex_linklist;
extern pthread_mutex_t mutex_slinklist;
extern pthread_mutex_t mutex_analysis;
extern pthread_mutex_t mutex_global;
extern pthread_mutex_t mutex_sms;
extern pthread_mutex_t mutex_buzzer;

extern pthread_cond_t cond_analysis;
extern pthread_cond_t cond_sqlite;
extern pthread_cond_t cond_refresh;
extern pthread_cond_t cond_buzzer;
extern pthread_cond_t cond_sms;

char tem_alarm_status = 0;
char hum_alarm_status = 0;
char ill_alarm_status = 0;
char beep_status = 0;

extern int msgid;
extern int dev_buzzer_mask;
extern struct storage_env_info storage_RT;

float dota_atof(char unitl)
{
	int tmp;
	float res=0.0;
	while(unitl)
	{
		tmp = unitl%10;
		unitl/=10;
		res = res/10+tmp;	
	}
	res/=10;
	return res;
}

int dota_atoi(const char *cDecade)
{
	int result = 0;
	if(' ' != cDecade[0])
		result = (cDecade[0]-48)*10;
	result += cDecade[1] -48;
	return result;
}

float dota_adc(unsigned int ratio)
{
	return ((ratio *3.6)/1024);
}

int storageAllgood(void)
{
	if((tem_alarm_status+hum_alarm_status+ill_alarm_status) == 0)
		return 0;
	return 1;
}

int sendMsgQueue(int witch,int do_what)
{
	struct msg msgbuf;
	int res;
	msgbuf.msgtype = witch;
	msgbuf.text[0] = do_what;

	res = msgsnd(msgid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);
	if(res != 0)
		return -1;
	return 0;
}

int checkEnv(struct storage_env_info *p)
{
	char flag = 0;
	static char A9_beep_status = 0;

	if (0 == tem_alarm_status)
	{
		if (p->temperature > p->temperatureMAX)
		{
			sendMsgQueue (MSG_LEDT, MSG_M0_LEDT_ON);
			sendMsgQueue (MSG_FAN, MSG_M0_FAN_ON3);
			p->fan_status = 3;
			tem_alarm_status = 2;
			flag = 1;
		}
		else if (p->temperature < p->temperatureMIN)
		{
			sendMsgQueue (MSG_LEDT, MSG_M0_LEDT_ON);
			sendMsgQueue (MSG_FAN, MSG_M0_FAN_OFF);
			p->fan_status = 0;
			tem_alarm_status = 1;
			flag = 1;
		}

		if (flag)
		{
			/*	pthread_mutex_lock (&mutex_sms);
				dev_sms_mask = SMS_TEM;
				pthread_mutex_unlock (&mutex_sms);
				pthread_cond_signal (&cond_sms);
				*/
			flag = 0;
			if (beep_status == 0)
			{
				beep_status = 1;
				sendMsgQueue (MSG_LEDT, MSG_M0_LEDT_ON);
				sendMsgQueue (MSG_BEEP, MSG_M0_BEEP_ON);
			}
		}
	}
	else
	{
		if (p->temperature < p->temperatureMAX && p->temperature > p->temperatureMIN)
		{
			sendMsgQueue (MSG_LEDT, MSG_M0_LEDT_OFF);
			sendMsgQueue (MSG_FAN, MSG_M0_FAN_OFF);
			p->fan_status = 0;
			tem_alarm_status = 0;
			if (!storageAllgood ())
			{
				beep_status = 0;
				sendMsgQueue (MSG_BEEP, MSG_M0_BEEP_OFF);
			}
		}
	}
	if (0 == hum_alarm_status)
	{
		if (p->humidity > p->humidityMAX)
		{
			sendMsgQueue (MSG_LEDH, MSG_M0_LEDH_ON);
			sendMsgQueue (MSG_FAN, MSG_M0_FAN_ON3);
			p->fan_status = 3;
			hum_alarm_status = 2;
			flag = 1;
		}
		else if (p->humidity < p->humidityMIN)
		{
			sendMsgQueue (MSG_LEDH, MSG_M0_LEDH_ON);
			sendMsgQueue (MSG_FAN, MSG_M0_FAN_OFF);
			p->fan_status = 0;
			hum_alarm_status = 1;
			flag = 1;
		}

		if (flag)
		{
			/*	pthread_mutex_lock (&mutex_sms);
				dev_sms_mask = SMS_HUM;
				pthread_mutex_unlock (&mutex_sms);
				pthread_cond_signal (&cond_sms);
				*/
			flag = 0;
			if (beep_status == 0)
			{
				beep_status = 1;
				sendMsgQueue (MSG_LEDH, MSG_M0_LEDH_ON);
				sendMsgQueue (MSG_BEEP, MSG_M0_BEEP_ON);
			}
		}
	}
	else
	{
		if (p->humidity < p->humidityMAX && p->humidity > p->humidityMIN)
		{
			sendMsgQueue (MSG_LEDH, MSG_M0_LEDH_OFF);
			sendMsgQueue (MSG_FAN, MSG_M0_FAN_OFF);
			p->fan_status = 0;
			hum_alarm_status = 0;
			if (!storageAllgood ())
			{
				beep_status = 0;
				sendMsgQueue (MSG_BEEP, MSG_M0_BEEP_OFF);
			}
		}
	}
	if (0 == ill_alarm_status)
	{
		if (p->illumination > p->illuminationMAX)
		{
			sendMsgQueue (MSG_LEDI, MSG_M0_LEDI_ON);
			p->led_status = 0;
			ill_alarm_status = 2;
			flag = 1;
		}
		else if (p->illumination < p->illuminationMIN)
		{
			sendMsgQueue (MSG_LEDI, MSG_M0_LEDI_ON);
			p->led_status = 1;
			ill_alarm_status = 1;
			flag = 1;
		}

		if (flag)
		{
			/*
			   pthread_mutex_lock (&mutex_sms);
			   dev_sms_mask = SMS_ILL;
			   pthread_mutex_unlock (&mutex_sms);
			   pthread_cond_signal (&cond_sms);
			   */
			flag = 0;
			if (beep_status == 0)
			{
				beep_status = 1;
				sendMsgQueue (MSG_LEDI, MSG_M0_LEDI_ON);
				sendMsgQueue (MSG_BEEP, MSG_M0_BEEP_ON);
			}

		}
	}
	else if (ill_alarm_status)
	{
		if (p->illumination < p->illuminationMAX && p->illumination > p->illuminationMIN)
		{
			sendMsgQueue (MSG_LEDI, MSG_M0_LEDI_OFF);
			p->led_status = 0;
			ill_alarm_status = 0;
			if (!storageAllgood ())
			{
				beep_status = 0;
				sendMsgQueue (MSG_BEEP, MSG_M0_BEEP_OFF);

			}
		}
		return 0;
	}
}
void getEnvPackage (link_datatype *buf)
{
	struct env_info_package pack;


	memcpy (&pack, buf->text, 20);
	int sto_no = pack.sto_no;

	pthread_mutex_lock (&mutex_global);
	struct storage_env_info current = storage_RT;
	pthread_mutex_unlock (&mutex_global);

	current.storage_status = 1;
	current.x = pack.x;
	current.y = pack.y;
	current.z = pack.z;
	current.temperature = pack.tem[0] + dota_atof (pack.tem[1]);
	current.humidity = pack.hum[0] + dota_atof (pack.hum[1]);
	current.illumination = pack.ill;
	current.battery = dota_adc(pack.battery);
	current.adc = dota_adc(pack.adc);

	printf ("no = %d tem = %0.2f hum = %0.2f ill = %0.2f battery = %0.2f adc = %0.2f x = %d y %d z = %d\n", sto_no,
			current.temperature, current.humidity, current.illumination, current.battery, current.adc, current.x, current.y, current.z);

	printf("%s:  %s: _____%d__\n",__FILE__,__func__,__LINE__);
//	checkEnv (&current);	
	printf("%s:  %s: _____%d__\n",__FILE__,__func__,__LINE__);

	pthread_mutex_lock (&mutex_global);
	printf("%s:  %s: _____%d__\n",__FILE__,__func__,__LINE__);
	storage_RT = current;
	pthread_mutex_lock (&mutex_slinklist);
	printf("%s:  %s: _____%d__\n",__FILE__,__func__,__LINE__);
	sqlite_InsertLinknode(COLLECT_INSERTER, storage_RT, sto_no, 0);
	printf("%s:  %s: _____%d__\n",__FILE__,__func__,__LINE__);
	pthread_mutex_unlock (&mutex_slinklist);
	pthread_mutex_unlock (&mutex_global);

	printf("%s:  %s: _____%d__\n",__FILE__,__func__,__LINE__);
	pthread_cond_signal (&cond_refresh);
	pthread_cond_signal (&cond_sqlite);
	printf("%s:  %s: _____%d__\n",__FILE__,__func__,__LINE__);
	return ;
}



void *pthread_analysis(void *arg)
{
	int sto_no = STO_NO;

	printf("this is pthread_analysis\n");
	linklist node = NULL;
	link_datatype buf;
	printf("linkHead = %p __%d__\n ",linkHead,__LINE__);

	while(1)
	{
		printf("%s:  %s: _____%d__\n",__FILE__,__func__,__LINE__);
		pthread_mutex_lock(&mutex_analysis);
		pthread_cond_wait(&cond_analysis, &mutex_analysis);
		pthread_mutex_unlock(&mutex_analysis);
		printf("%s:  %s: _____%d__\n",__FILE__,__func__,__LINE__);

		while (1)
		{
			pthread_mutex_lock(&mutex_linklist);
			printf("%s:  %s: _____%d__\n",__FILE__,__func__,__LINE__);
			if ((node = GetLinknode(linkHead)) == NULL)
			{
				pthread_mutex_unlock(&mutex_linklist);
				break;
			}
			printf("node = %p __%d__\n ",node,__LINE__);
			printf("%s:  %s: _____%d__\n",__FILE__,__func__,__LINE__);
			buf = node->buf;
			free (node);
			node = NULL;
			pthread_mutex_unlock (&mutex_linklist);
			if ('e' == buf.msg_type)
			{
				printf("%s:  %s: _____%d__\n",__FILE__,__func__,__LINE__);
				getEnvPackage (&buf);
			}
			else if ('u' == buf.msg_type)
			{
				printf("%s:  %s: _____%d__\n",__FILE__,__func__,__LINE__);
				pthread_mutex_lock (&mutex_slinklist);
				sqlite_InsertLinknode (STORAGE_UPDATE, storage_RT, sto_no, 0);
				printf("%s:  %s: __%d__shead: %p\n",__FILE__,__func__,__LINE__,slinkHead);
				pthread_mutex_unlock (&mutex_slinklist);
				pthread_cond_signal (&cond_sqlite);

			}
		}
	printf("%s:  %s: _____%d__\n",__FILE__,__func__,__LINE__);
	}
	pthread_exit(NULL);
}

