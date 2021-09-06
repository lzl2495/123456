#include "global.h"


void *pthread_refresh(void *arg)
{

	printf("this is pthread_refresh\n");

	pthread_exit(NULL);

}

