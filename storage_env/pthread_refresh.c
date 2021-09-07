#include "global.h"
#include "sqlite_link_list.h"
#include <sys/sem.h>
#include <sys/shm.h>

#define N 1024

extern pthread_mutex_t mutex_refresh;
extern pthread_mutex_t mutex_global;
extern pthread_mutex_t mutex_slinklist;
extern pthread_cond_t cond_refresh;
extern pthread_cond_t cond_sqlite;

int shmid; 
int semid;
extern struct storage_env_info storage_RT ;

void *pthread_refresh(void *arg)
{

	key_t key_info;
	int shmid, semid;

	struct storage_env_info *shm_buf;
	struct sembuf sop;

	if((key_info = ftok ("/home", 1)) < 0)
	{
		perror ("ftok info");
		exit (-1);
	}
	printf ("key = %d\n", key_info);

	if ((semid = semget (key_info, 2, IPC_CREAT|0666)) < 0)
	{
		perror ("semget");
		exit (-1);
	}
	semctl(semid,0,SETVAL,0);
	semctl(semid,1,SETVAL,1);

	if ((shmid = shmget(key_info, sizeof(storage_RT), IPC_CREAT | 0666)) < 0)
	{
		perror ("shmget");
		exit (-1);
	}
	if ((shm_buf = (struct storage_env_info *)shmat(shmid, NULL, 0)) == (void *)-1)
	{
		perror ("shmat");
		exit (-1);
	}
	bzero (shm_buf, sizeof (struct storage_env_info));
	printf("this is pthread_refresh\n");

	while(1)
	{
		pthread_mutex_lock (&mutex_refresh);
		pthread_cond_wait (&cond_refresh, &mutex_refresh);
		sop.sem_num = 1;
		sop.sem_op = -1;
		sop.sem_flg = 0;
		semop(semid,&sop,1);
		pthread_mutex_lock (&mutex_global);
		*shm_buf = storage_RT;
		pthread_mutex_unlock (&mutex_global);
		sop.sem_num = 0;
		sop.sem_op = 1;
		sop.sem_flg = 0;
		semop(semid,&sop,1);
		pthread_mutex_unlock (&mutex_refresh);

	}

	pthread_exit(NULL);

}

