#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <dirent.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/epoll.h>

#define ERR_MSG(msg) do{ \
	printf("__%d__,%s\n",__LINE__,__func__);\
	perror(msg);\
}while(0);


#define ROOT "0000"
#define LOGIN 3
#define QUIT 1
#define CLEAR 2

#define USER_MODIFY 3
#define USER_SEARCH 4
#define USER_ADD 5
#define USER_DEL 6

#define MODIFY_PASSWORD 3
#define MODIFY_PHONE 4
#define MODIFY_ADDRESS 5
#define MODIFY_DEPARTMENT 6
#define MODIFY_SALARY 7


#define IP "0.0.0.0"
#define PORT 8888
#define N 24
#define BUF_SIZE 256
#define SQL_SIZE 512

typedef struct 
{
	int newfd;
	struct sockaddr_in cin;
	sqlite3 *db_user;
}_MSG;

typedef struct 
{
	int type;
	char id[N];
	char password[N];
}_USER;

typedef struct
{
	char id[N];
	char name[N];
	char age[N];
	char sex[N];
	char department[N];	
	char salary[N];
	char phone[N];
	char address[N];
	char update_key[N];
	char update_msg[N];

}_USER_MSG;

sqlite3 *init_user_db(void);
int init_socket(int argc,const char *argv[]);
int do_login(int newfd,sqlite3 *db_user);
int do_return(sqlite3 *db_user,_USER user);
void *dict_pthread(void *arg);
int do_registration(int newfd, sqlite3 *db_user,_USER user);
int do_delete(int newfd, sqlite3 *db_user,_USER user);
int do_search(int newfd, sqlite3 *db_user,_USER user);
int do_modify(int newfd,sqlite3 *db_user,_USER user);
int do_check_user(sqlite3 *db_user,_USER_MSG user_msg);
int do_modify_password(int newfd, sqlite3 *db_user,_USER user);
int do_modify_msg(int newfd,sqlite3 *db_user,_USER user);

int main(int argc, const char *argv[])
{
	if(argc < 3)
	{
		printf("请输入IP和端口\n");
		return -1;
	}

	//初始化数据库
	sqlite3 *db_user;
	db_user = init_user_db();

	//初始化网络
	int sfd = init_socket(argc,argv);
	if(sfd < 0)
	{
		ERR_MSG("socket");
		return -1;
	}

	struct sockaddr_in cin;
	socklen_t addrlen = sizeof(cin);
	pthread_t tid;

	//多线程循环服务器
	while(1)
	{
		//等待获取新的套接字
		int newfd = accept(sfd,(struct sockaddr*)&cin,&addrlen);
		if(newfd < 0)
		{
			ERR_MSG("accept");
			return -1;
		}
		//	printf("newfd = %d\n",newfd);
		printf("[%s:%d]: %d 已上线\n",inet_ntoa(cin.sin_addr),ntohs(cin.sin_port),newfd);

		//结构体初始化，给分支线程传参
		_MSG cli_info;
		cli_info.newfd = newfd;
		cli_info.cin = cin;
		cli_info.db_user = db_user;

		//创建多线程
		if(pthread_create(&tid,NULL,dict_pthread,(void*)&cli_info)!=0)
		{
			ERR_MSG("pthread_create");
			return -1;
		}
	}

	close(sfd);
	sqlite3_close(db_user);

	return 0;
}


//分支线程{{{
void *dict_pthread(void *arg)
{
	//线程分离
	pthread_detach(pthread_self());

	_MSG cli_info = *(_MSG*)arg;
	int newfd = cli_info.newfd;
	struct sockaddr_in cin = cli_info.cin;
	sqlite3 *db_user = cli_info.db_user;

	char buf[BUF_SIZE];
	ssize_t res = 0;
	while(1)
	{
		bzero(buf,sizeof(buf));
		//接收操作指令
		res = recv(newfd, buf,sizeof(buf),0);
		if(res < 0)
			{
			ERR_MSG("recv");
			break;
			}
			else if(res <= 0)
		{
			printf("*****客户端%d关闭*****__%d__\n",newfd,__LINE__);
			break;
		}

		//判断指令
		switch(buf[0])
		{
		case LOGIN:
			//登录
			printf("[%s:%d]:请求登录\n",inet_ntoa(cin.sin_addr),ntohs(cin.sin_port));
			do_login(newfd,db_user);
			break;
		case QUIT:
			//退出
			printf("[%s:%d]:已退出登录\n",inet_ntoa(cin.sin_addr),ntohs(cin.sin_port));
			break;
		default:
			break;
		}
	}


	close(newfd);
	pthread_exit(NULL);
}
//}}}


//用户登录{{{
int do_login(int newfd,sqlite3 *db_user)
{
	_USER user;
	char buf[BUF_SIZE]= "";
	char sql[SQL_SIZE] = "";
	char *errmsg;
	//接收客户端发过来的信息
	int res = recv(newfd,(void*)&user,sizeof(user),0);
	if(res < 0)
	{
		ERR_MSG("recv");
		return -1;
	}

	printf("id = %s , password = %s \n",user.id,user.password);
	printf("%s:__%d__\n",__func__,__LINE__);
	//从数据库中得到该id的用户注册信息，用于对比
	int row ,column ,i= 0;
	char **pazresult= NULL;
	bzero(buf,sizeof(buf));
	sprintf(sql,"select password ,stage from user where id = '%s'",user.id);
	printf("sql = %s __%d__\n",sql,__LINE__);
	res = sqlite3_get_table(db_user,sql,&pazresult,&row,&column,&errmsg);
	if(res != SQLITE_OK)
	{
		printf("%d   %s\n",__LINE__,errmsg);
		sqlite3_free(errmsg);
		sqlite3_free_table(pazresult);
		return -1;
	}
	if(row > 0)
	{
		if(strcmp(user.password,pazresult[2]) == 0)
		{
			if(strcmp(pazresult[3],"0") == 0)
			{
				//判定成功
				//	printf("登陆成功\n");
			}
			else
			{
				//该用户已登录
				//	printf("该用户已登录\n");
				bzero(buf,sizeof(buf));
				strcpy(buf,"exists");
				send(newfd,buf,sizeof(buf),0);
				sqlite3_free_table(pazresult);
				return -1;
			}
		}
		else
		{
			//密码错误
			//	printf("密码错误\n");
			bzero(buf,sizeof(buf));
			strcpy(buf,"password error");
			send(newfd,buf,sizeof(buf),0);
			sqlite3_free_table(pazresult);
			return -1;
		}
	}
	else
	{
		//用户不存在
		//	printf("用户不存在\n");
		bzero(buf,sizeof(buf));
		strcpy(buf,"name error");
		send(newfd,buf,sizeof(buf),0);
		sqlite3_free_table(pazresult);
		return -1;
	}
	sqlite3_free_table(pazresult);

	sprintf(sql,"update user set stage = 1 where id = '%s' and password = '%s'",user.id,user.password);
	printf("sql = %s __%d__\n",sql,__LINE__);
	res = sqlite3_exec(db_user,sql,NULL,NULL,&errmsg);
	if(res != 0)
	{
		bzero(buf,sizeof(buf));
		strcpy(buf,"error");
		send(newfd,buf,sizeof(buf),0);

		printf("%d   %s\n",__LINE__,errmsg);
		sqlite3_free(errmsg);
		return -1;
	}

	printf("登陆成功\n");
	bzero(buf,sizeof(buf));
	strcpy(buf,"success");
	send(newfd,buf,sizeof(buf),0);

	//接收用户指令
	while(1)
	{
		bzero(buf,sizeof(buf));
		//接收操作指令
		res = recv(newfd,buf,N,0);
		if(res < 0)
		{
			do_return(db_user,user);
			ERR_MSG("recv");
			break;
		}
		else if(res == 0)
		{
			do_return(db_user,user);
			break;
		}

		//判断指令
		switch(buf[0])
		{
		case USER_ADD:
			//新增
			printf("%s:请求新增员工\n",user.id);
			do_registration(newfd,db_user,user);
			break;
		case USER_DEL:
			//删除
			printf("%s:请求删除员工\n",user.id);
			do_delete(newfd,db_user,user);
			break;
		case USER_MODIFY:
			//修改
			printf("%s:请求修改修改信息\n",user.id);
			do_modify(newfd,db_user,user);
			break;
		case USER_SEARCH:
			//查询
			printf("%s:请求查询员工信息\n",user.id);
			do_search(newfd,db_user,user);
			break;
		case QUIT:
			//返回上级目录
			printf("%s:请求返回上级目录\n",user.id);
			do_return(db_user,user);
			return 0;
		default:
			break;
		}
	}
	return 0;
}
//}}}

//用户注册{{{
int do_registration(int newfd, sqlite3 *db_user,_USER user)
{

	char buf[BUF_SIZE]= "";
	char sql[SQL_SIZE] = "";
	_USER_MSG user_msg;
	char *errmsg;
	//接收客户端发过来的信息
	int res = recv(newfd,(void*)&user_msg,sizeof(user_msg),0);
	if(res < 0)
	{
		ERR_MSG("recv");
		return -1;
	}
	else if(res == 0)
	{
		do_return(db_user,user);
		return -1;
	}


	sprintf(buf,"'%s','123456','0'",user_msg.id);
	sprintf(sql,"insert into user values(%s)",buf);
	printf("sql = %s __%d__\n",sql,__LINE__);
	res = sqlite3_exec(db_user,sql,NULL,NULL,&errmsg);
	if(res != 0)
	{
		bzero(buf,sizeof(buf));
		strcpy(buf,"exists");
		send(newfd,buf,sizeof(buf),0);

		printf("%d   %s\n",__LINE__,errmsg);
		sqlite3_free(errmsg);
		return -1;
	}

	bzero(buf,sizeof(buf));
	bzero(sql,sizeof(sql));
	sprintf(buf,"'%s','%s',%s,'%s','%s',%s,'%s','%s'",user_msg.id,user_msg.name,
			user_msg.age,user_msg.sex,user_msg.department,user_msg.salary,user_msg.phone,
			user_msg.address);
	sprintf(sql,"insert into user_msg values(%s)",buf);
	printf("sql = %s __%d__\n",sql,__LINE__);
	res = sqlite3_exec(db_user,sql,NULL,NULL,&errmsg);
	if(res != 0)
	{
		printf("%d   %s\n",__LINE__,errmsg);
		sqlite3_free(errmsg);
		return -1;
	}

	bzero(buf,sizeof(buf));
	strcpy(buf,"success");
	send(newfd,buf,sizeof(buf),0);

	return 0;
}
//}}}
//
//用户删除{{{
int do_delete(int newfd, sqlite3 *db_user,_USER user)
{

	char buf[BUF_SIZE]= "";
	char sql[SQL_SIZE] = "";
	char *errmsg;
	_USER_MSG user_msg;
	//接收客户端发过来的信息
	int res = recv(newfd,(void*)&user_msg,sizeof(user_msg),0);
	if(res < 0)
	{
		ERR_MSG("recv");
		return -1;
	}
	else if(res == 0)
	{
		do_return(db_user,user);
		return -1;
	}
	//检查表中是否有该用户
	res = do_check_user(db_user,user_msg);
	if(res == 0)
	{
		sprintf(sql,"delete from user where id = '%s'",user.id);
		printf("sql = %s __%d__\n",sql,__LINE__);
		res = sqlite3_exec(db_user,sql,NULL,NULL,&errmsg);
		if(res != 0)
		{
			printf("%d   %s\n",__LINE__,errmsg);
			sqlite3_free(errmsg);
			return -1;
		}

		bzero(sql,sizeof(sql));
		sprintf(sql,"delete from user_msg where id = '%s'",user.id);
		printf("sql = %s __%d__\n",sql,__LINE__);
		res = sqlite3_exec(db_user,sql,NULL,NULL,&errmsg);
		if(res != 0)
		{
			printf("%d   %s\n",__LINE__,errmsg);
			sqlite3_free(errmsg);
			return -1;
		}

		bzero(buf,sizeof(buf));
		strcpy(buf,"success");
		send(newfd,buf,sizeof(buf),0);
	}
	//没有
	else if(res < 0)
	{
		bzero(buf,sizeof(buf));
		strcpy(buf,"none");
		send(newfd,buf,sizeof(buf),0);
		return -1;;
	}



	return 0;
}
//}}}
//
//查询{{{
int do_search(int newfd, sqlite3 *db_user,_USER user)
{
	char buf[BUF_SIZE]= "";
	char sql[SQL_SIZE] = "";
	_USER_MSG user_msg;
	char *errmsg;
	int row ,column ,i= 0;
	char **pazresult= NULL;
	memset(&user_msg,0 ,sizeof(user_msg));
	//接收客户端发过来的信息
	printf("%s:__%d__\n",__func__,__LINE__);
	int res = recv(newfd,(void*)&user_msg,sizeof(user_msg),0);
	printf("%s:__%d__\n",__func__,__LINE__);
	if(res < 0)
	{
		ERR_MSG("recv");
		return -1;
	}
	else if(res == 0)
	{
		do_return(db_user,user);
		return -1;
	}
	printf("%s:__%d__\n",__func__,__LINE__);
	if(strcmp(user_msg.id,"*") == 0 && user.type == 1)
		sprintf(sql,"select * from user_msg");
	else
		sprintf(sql,"select * from user_msg where id = '%s' or name = '%s'",user_msg.id,user_msg.id);
	printf("sql = %s __%d__\n",sql,__LINE__);
	res = sqlite3_get_table(db_user,sql,&pazresult,&row,&column,&errmsg);
	if(res != SQLITE_OK)
	{
		fprintf(stderr,"%d   %s\n",__LINE__,errmsg);
		sqlite3_free(errmsg);
		sqlite3_free_table(pazresult);
		return -1;
	}
	//有
	if(row > 0)
	{
		for(i  = column; i < (row+1)*column ; i+=column)
		{
			//	printf("%s :%s\n",word_buf,pazresult[1]);
			strcpy(user_msg.id,pazresult[i]);
			strcpy(user_msg.name,pazresult[i+1]);
			strcpy(user_msg.age,pazresult[i+2]);
			strcpy(user_msg.sex,pazresult[i+3]);
			strcpy(user_msg.department,pazresult[i+4]);
			strcpy(user_msg.salary,pazresult[i+5]);
			strcpy(user_msg.phone,pazresult[i+6]);
			strcpy(user_msg.address,pazresult[i+7]);
			send(newfd,(void*)&user_msg,sizeof(user_msg),0);
			memset(&user_msg,0 ,sizeof(user_msg));
		}

		//发送完成提示
		memset(&user_msg,0 ,sizeof(user_msg));
		strcpy(user_msg.id,"success");
		send(newfd,(void*)&user_msg,sizeof(user_msg),0);

	}
	//没有
	else
	{
		memset(&user_msg,0 ,sizeof(user_msg));
		strcpy(user_msg.id,"empty");
		send(newfd,(void*)&user_msg,sizeof(user_msg),0);
		return -1;;
	}
	return 0;
}
//}}}

//修改信息{{{
int do_modify(int newfd,sqlite3 *db_user,_USER user)
{	
	char buf[BUF_SIZE]= "";
	char sql[SQL_SIZE] = "";
	_USER_MSG user_msg;
	char *errmsg;
	ssize_t res = 0;
	memset(&user_msg,0 ,sizeof(user_msg));

	//接收用户指令
	while(1)
	{
		bzero(buf,sizeof(buf));
		//接收操作指令
		res = recv(newfd,buf,N,0);
		if(res < 0)
		{
			ERR_MSG("recv");
			break;
		}
		else if(res == 0)
		{
			do_return(db_user,user);
			break;
		}

		//判断指令
		switch(buf[0])
		{
		case MODIFY_PASSWORD:
			//修改密码
			printf("%s:请求修改员工密码\n",user.id);
			do_modify(newfd,db_user,user);
			do_modify_password(newfd,db_user,user);
			break;
		case MODIFY_PHONE:
			//修改电话
			printf("%s:请求修改员工电话\n",user.id);
			do_modify_msg(newfd,db_user,user);

			break;
		case MODIFY_ADDRESS:
			//修改住址
			printf("%s:请求修改员工住址\n",user.id);
			do_modify_msg(newfd,db_user,user);
			break;
		case MODIFY_DEPARTMENT:
			//修改部门
			printf("%s:请求修改员工部门\n",user.id);
			do_modify_msg(newfd,db_user,user);
			break;
		case MODIFY_SALARY:
			//修改薪资
			printf("%s:请求修改员工薪资\n",user.id);
			do_modify_msg(newfd,db_user,user);
			break;
		case QUIT:
			//返回上级目录
			printf("%s:请求返回上级目录\n",user.id);
			return 0;
		default:
			break;
		}
	}
	return 0;


}

//检查表中是否有该用户{{{
int do_check_user(sqlite3 *db_user,_USER_MSG user_msg)
{
	char buf[BUF_SIZE]= "";
	char sql[SQL_SIZE] = "";
	char *errmsg;
	int row ,column ,i= 0;
	char **pazresult= NULL;
	//接收客户端发过来的信息
	sprintf(sql,"select * from user_msg where id = '%s'",user_msg.id);
	printf("sql = %s __%d__\n",sql,__LINE__);
	int res = sqlite3_get_table(db_user,sql,&pazresult,&row,&column,&errmsg);
	if(res != SQLITE_OK)
	{
		fprintf(stderr,"%d   %s\n",__LINE__,errmsg);
		sqlite3_free(errmsg);
		sqlite3_free_table(pazresult);
		return -1;
	}
	//有
	if(row > 0)
	{
		return 0;
	}
	//没有
	else
	{
		return -1;;
	}
	return 0;
}



int do_modify_password(int newfd,sqlite3* db_user,_USER user)
{
	char buf[BUF_SIZE]= "";
	char sql[SQL_SIZE] = "";
	char *errmsg;
	int row ,column ;
	char **pazresult= NULL;
	memset(&user,0 ,sizeof(user));
	//接收客户端发过来的信息
	int res = recv(newfd,(void*)&user,sizeof(user),0);
	if(res < 0)
	{
		ERR_MSG("recv");
		return -1;
	}
	else if(res == 0)
	{
		do_return(db_user,user);
		return -1;
	}
	printf("id =  %s __%d__\n",user.id,__LINE__);
	printf("password =  %s __%d__\n",user.password,__LINE__);
	//检查表中有没有该用户
	sprintf(sql,"select * from user where id = '%s'",user.id);
	printf("sql = %s __%d__\n",sql,__LINE__);
	res = sqlite3_get_table(db_user,sql,&pazresult,&row,&column,&errmsg);
	if(res != SQLITE_OK)
	{
		fprintf(stderr,"%d   %s\n",__LINE__,errmsg);
		sqlite3_free(errmsg);
		sqlite3_free_table(pazresult);
		return -1;
	}
	//有
	if(row > 0)
	{
		sprintf(sql,"update user set password = '%s' where id = '%s'",user.password,user.id);
		printf("sql = %s __%d__\n",sql,__LINE__);
		res = sqlite3_exec(db_user,sql,NULL,NULL,&errmsg);
		if(res != 0)
		{
			printf("%d   %s\n",__LINE__,errmsg);
			sqlite3_free(errmsg);
			return -1;
		}

		bzero(buf,sizeof(buf));
		strcpy(buf,"success");
		send(newfd,buf,sizeof(buf),0);

	}
	//没有
	else
	{
		bzero(buf,sizeof(buf));
		strcpy(buf,"none");
		send(newfd,buf,sizeof(buf),0);
		return -1;;
	}
	return 0;

}

int do_modify_msg(int newfd,sqlite3 *db_user,_USER user)
{
	char buf[BUF_SIZE]= "";
	char sql[SQL_SIZE] = "";
	char *errmsg;
	_USER_MSG user_msg;
	memset(&user_msg,0 ,sizeof(user_msg));
	//接收客户端发过来的信息
	int res = recv(newfd,(void*)&user_msg,sizeof(user_msg),0);
	if(res < 0)
	{
		ERR_MSG("recv");
		return -1;
	}
	else if(res == 0)
	{
		do_return(db_user,user);
		return -1;
	}

	res = do_check_user(db_user,user_msg);
	if(res == 0)
	{
		sprintf(sql,"update user_msg set %s = '%s' where id = '%s'",user_msg.update_key,user_msg.update_msg
				,user_msg.id);
		printf("sql = %s __%d__\n",sql,__LINE__);
		res = sqlite3_exec(db_user,sql,NULL,NULL,&errmsg);
		if(res != 0)
		{
			printf("%d   %s\n",__LINE__,errmsg);
			sqlite3_free(errmsg);
			return -1;
		}

		bzero(buf,sizeof(buf));
		strcpy(buf,"success");
		send(newfd,buf,sizeof(buf),0);

	}
	//没有
	else if(res < 0)
	{
		bzero(buf,sizeof(buf));
		strcpy(buf,"none");
		send(newfd,buf,sizeof(buf),0);
		return -1;;
	}



	return 0;
}
//}}}

//初始化用户数据库{{{
sqlite3 *init_user_db(void)
{
	sqlite3 *db_user;
	//打开或创建用户数据库
	int	res = sqlite3_open("./user.db",&db_user);
	if(res != SQLITE_OK)
	{
		printf("打开用户数据库失败__%d__\n",__LINE__);
		exit(0);
	}
	printf("创建用户数据库成功\n");

	char *errmsg;
	char sql[SQL_SIZE] = "";
	char buf[BUF_SIZE]= "";

	//创建用户注册登录表
	bzero(sql,sizeof(sql));
	strcpy(sql,"create table if not exists user(id char primary key, password char\
		, stage int)");
	res = sqlite3_exec(db_user,sql,NULL,NULL,&errmsg);
	if(res != SQLITE_OK)
	{
		sqlite3_close(db_user);
		printf("%d   %s\n",__LINE__,errmsg);
		sqlite3_free(errmsg);
		exit(0);
	}
	printf("用户注册表单创建成功\n");
	sprintf(sql,"insert into user values('0000','123456','0')");
	res = sqlite3_exec(db_user,sql,NULL,NULL,&errmsg);


	//创建用户信息表
	bzero(sql,sizeof(sql));
	strcpy(sql,"create table if not exists user_msg(id char primary key,name char\
		,age int,sex char,department char ,salary int ,phone char ,address char)");
	res = sqlite3_exec(db_user,sql,NULL,NULL,&errmsg);
	if(res != SQLITE_OK)
	{
		sqlite3_close(db_user);
		printf("%d   %s\n",__LINE__,errmsg);
		sqlite3_free(errmsg);
		exit(0);
	}
	printf("用户信息表单创建成功\n");



	//清空用户登录表
	bzero(sql,sizeof(sql));
	strcpy(sql,"update user set stage = 0");
	res = sqlite3_exec(db_user,sql,NULL,NULL,&errmsg);
	if(res != SQLITE_OK)
	{
		sqlite3_close(db_user);
		printf("%d   %s\n",__LINE__,errmsg);
		sqlite3_free(errmsg);
		exit(0);
	}
	printf("用户登录状态已清空\n");

	return db_user;

}
//}}}

//返回上目录{{{
int do_return(sqlite3 *db_user,_USER user)
{
	char sql[SQL_SIZE] = "";
	char *errmsg;
	int row ,column ,i= 0;
	char **pazresult= NULL;
	//组装sql语句
	sprintf(sql,"update user set stage = 0 where id = '%s'",user.id);
	printf("sql = %s __%d__\n",sql,__LINE__);
	//调用函数
	int res = sqlite3_exec(db_user,sql,NULL,NULL,&errmsg);
	if(res != SQLITE_OK)
	{
		sqlite3_close(db_user);
		printf("%d   %s\n",__LINE__,errmsg);
		sqlite3_free(errmsg);

	}
	printf("用户状态修改成功\n");

	return 0;

}
//}}}





//初始化网络{{{
int init_socket(int argc,const char *argv[])
{

	if(argc < 3)
	{
		printf("请输入IP和端口\n");
		return -1;
	}

	//创建套接字
	int sfd = socket(AF_INET,SOCK_STREAM,0);
	if(sfd < 0)
	{
		ERR_MSG("socket");
		return -1;
	}

	int setvalue = 1;
	if(setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR,&setvalue,sizeof(setvalue))<0)
	{
		ERR_MSG("setsockopt");
		return -1;
	}

	//绑定服务器ip和端口
	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port   = htons(atoi(argv[2]));
	sin.sin_addr.s_addr = inet_addr(argv[1]);
	if(bind(sfd,(struct sockaddr*)&sin,sizeof(sin)) < 0)
	{
		ERR_MSG("bind");
		return -1;
	}
	//将套接字设置为被动监听状态  
	if(listen(sfd,5) < 0)
	{
		ERR_MSG("listen");
		return -1;
	}
	printf("监听成功\n");

	return sfd;
}
//}}}

