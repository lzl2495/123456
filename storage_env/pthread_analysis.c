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

char tem_alarm_status;
char hum_alarm_status;
char ill_alarm_status;
char beep_status;

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

void *pthread_analysis(void *arg)
{

	printf("this is pthread_analysis\n");

	

	pthread_exit(NULL);

}

