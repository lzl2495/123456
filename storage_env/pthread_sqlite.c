#include "global.h"
#include <sqlite3.h>

#define SQL_SIZE 512
sqlite3 *db_storage;

int init_storage_db(void);
void *pthread_sqlite(void *arg)
{

	printf("this is pthread_sqlite\n");

	init_storage_db();


	pthread_exit(NULL);

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
	strcpy(sql,"create table if not exists user(temperatureMax float,temperatureMin float,\
		username char primary key, password char)");
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
	strcpy(sql,"create table if not exists user(temperatureVal float, humidityVal float,\
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


}


