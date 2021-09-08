#include "global.h"
#include <sqlite3.h>
#include "sqlite_link_list.h"

#define SQL_SIZE 512
sqlite3 *db_storage;
extern slinklist slinkHead;
extern pthread_cond_t cond_sqlite;    //数据库线程被唤醒条件变量
extern pthread_mutex_t mutex_sqlite;  //数据库线程互斥锁
extern pthread_mutex_t mutex_slinklist;  //数据库缓存互斥锁


int init_storage_db(void);
void sqlite_task(unsigned char type,struct storage_env_info env_buf);
void *pthread_sqlite(void *arg)
{

	slinklist node = NULL;
	printf("this is pthread_sqlite\n");

	init_storage_db();

	slinkHead = sqlite_linkCreate();
	printf("%s:  %s: __%d__shead: %p\n",__FILE__,__func__,__LINE__,slinkHead);
	if(slinkHead == NULL)
		return NULL;

	while(1)
	{
	printf("%s:  %s: _____%d__\n",__FILE__,__func__,__LINE__);
		pthread_mutex_lock(&mutex_sqlite);
		pthread_cond_wait(&cond_sqlite,&mutex_sqlite);
		pthread_mutex_unlock(&mutex_sqlite);

	printf("%s:  %s: _____%d__\n",__FILE__,__func__,__LINE__);
		while(1)
		{
	printf("%s:  %s: _____%d__\n",__FILE__,__func__,__LINE__);
			pthread_mutex_lock (&mutex_slinklist);
			if ((node = sqlite_GetLinknode (slinkHead)) == NULL)
			{
				pthread_mutex_unlock (&mutex_slinklist);
				break;
			}
	printf("%s:  %s: _____%d__\n",__FILE__,__func__,__LINE__);
	printf("%s:  %s: __%d__ node :%p\n",__FILE__,__func__,__LINE__,node);
			pthread_mutex_unlock (&mutex_slinklist);
			sqlite_task(node->type,node->env_buf);
			free(node);
			node = NULL;
	printf("%s:  %s: _____%d__\n",__FILE__,__func__,__LINE__);
		}
	}

	pthread_exit(NULL);

}

void sqlite_task(unsigned char type,struct storage_env_info env_buf)
{
	int res;
	char sql[SQL_SIZE] = "";
	char *errmsg;
	printf("%s:  %s: _____%d__\n",__FILE__,__func__,__LINE__);
	switch(type)
	{
	case COLLECT_INSERTER:
	printf("%s:  %s: _____%d__\n",__FILE__,__func__,__LINE__);
		bzero(sql,sizeof(sql));
		sprintf(sql,"insert into collect_env values(%f, %f,%f)",env_buf.temperature,env_buf.humidity,env_buf.illumination);
		printf("sql : %s __%d__%s\n",sql,__LINE__,__FILE__);
		res = sqlite3_exec(db_storage,sql,NULL,NULL,&errmsg);
		if(res != SQLITE_OK)
		{
			sqlite3_close(db_storage);
			printf("%d   %s\n",__LINE__,errmsg);
			sqlite3_free(errmsg);
			exit(0);
		}
		break;
	case STORAGE_UPDATE:
	printf("%s:  %s: _____%d__\n",__FILE__,__func__,__LINE__);
		bzero(sql,sizeof(sql));
		sprintf(sql,"update environment set temperatureMax =%f,temperatureMin =%f,\
				humidityMax =%f,humidityMin =%f,illuminationMax =%f,illuminationMin =%f where \
				sto_no = 1",env_buf.temperatureMAX,env_buf.temperatureMIN,env_buf.humidityMAX,\
				env_buf.humidityMIN,env_buf.illuminationMAX,env_buf.illuminationMIN);
		printf("sql : %s __%d__%s\n",sql,__LINE__,__FILE__);
		res = sqlite3_exec(db_storage,sql,NULL,NULL,&errmsg);
		if(res != SQLITE_OK)
		{
			sqlite3_close(db_storage);
			printf("%d   %s\n",__LINE__,errmsg);
			sqlite3_free(errmsg);
			exit(0);
		}
		break;
	case COLLECT_GET:
		break;
	case STORAGE_GET:
	default:
		break;

	}

}

int init_storage_db(void)
{
	//打开或创建数据库
	int	res = sqlite3_open("./storage.db",&db_storage);
	if(res != SQLITE_OK)
	{
		printf("打开数据库失败__%d__\n",__LINE__);
		exit(0);
	}
	printf("创建数据库成功\n");

	char *errmsg;
	char sql[SQL_SIZE] = "";

	//创建仓库环境表
	bzero(sql,sizeof(sql));
	strcpy(sql,"create table if not exists environment(temperatureMax float,temperatureMin float,\
		humidityMax float,humidityMin float,illuminationMax float,illuminationMin float,\
		sto_no int,username char primary key, password char)");
	res = sqlite3_exec(db_storage,sql,NULL,NULL,&errmsg);
	if(res != SQLITE_OK)
	{
		sqlite3_close(db_storage);
		printf("%d   %s\n",__LINE__,errmsg);
		sqlite3_free(errmsg);
		exit(0);
	}
	printf("仓库环境表单创建成功\n");

	//创建仓库实时温湿度表
	bzero(sql,sizeof(sql));
	strcpy(sql,"create table if not exists collect_env(temperatureVal float, humidityVal float,\
		illuminationVal float)");
	res = sqlite3_exec(db_storage,sql,NULL,NULL,&errmsg);
	if(res != SQLITE_OK)
	{
		sqlite3_close(db_storage);
		printf("%d   %s\n",__LINE__,errmsg);
		sqlite3_free(errmsg);
		exit(0);
	}
	printf("仓库实时温湿度表单创建成功\n");

	bzero(sql,sizeof(sql));
	strcpy(sql,"insert into environment values(50,10,80,20,500,10,1,'123','123')");
	res = sqlite3_exec(db_storage,sql,NULL,NULL,&errmsg);
	printf("初始化表单成功\n");

}


