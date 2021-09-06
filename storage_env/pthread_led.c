#include "global.h"


void *pthread_led(void *arg)
{

	printf("this is pthread_led\n");

	pthread_exit(NULL);

}

