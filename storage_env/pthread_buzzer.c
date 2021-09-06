#include "global.h"


void *pthread_buzzer(void *arg)
{

	printf("this is pthread_buzzer\n");

	pthread_exit(NULL);

}

