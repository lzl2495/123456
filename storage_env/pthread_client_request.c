#include "global.h"


void *pthread_client_request(void *arg)
{

	printf("this is pthread_client_request\n");

	pthread_exit(NULL);

}

