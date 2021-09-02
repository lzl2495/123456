#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sqlite3.h>
#include <signal.h>
#include <unistd.h>


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

#define N 24
#define BUF_SIZE 256
#define IP "0.0.0.0"
#define PORT 8888

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

typedef struct 
{
	int type;
	char id[N];
	char password[N];
}_USER;


_USER user_me;

int do_registration(int sfd);
int do_login(int sfd);
int do_enquiry(int sfd);
int do_root_or_user_op(int sfd,_USER user);
int do_user_add(int sfd,_USER user);
int do_user_del(int sfd,_USER user);
int do_user_search(int sfd,_USER user);
int user_modify(int sfd,_USER user);
int do_modify_password(int sfd,_USER user);
int do_modify_msg(int sfd,_USER user,int choose_num);

int main(int argc, const char *argv[])
{
	if(argc < 3)
	{
		printf("请输入IP和端口\n");
		return -1;
	}
	int sfd = socket(AF_INET,SOCK_STREAM,0);
	if(sfd < 0)
	{
		ERR_MSG("socket");
		return -1;
	}


	//填充服务器ip和端口
	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port   = htons(atoi(argv[2]));
	sin.sin_addr.s_addr = inet_addr(argv[1]);


	//连接申请
	if(connect(sfd,(struct sockaddr*)&sin,sizeof(sin))<0)
	{
		ERR_MSG("connect");
		return -1;
	}

	int choose_num= 0;
	//进入循环判断需要执行的程序
	while(1)
	{
		choose_num = 0;
		printf("\n**********************\n");
		printf("********1.退出********\n");
		printf("********2.清屏********\n");
		printf("********3.登录********\n");
		printf("**********************\n");
		//获取指令
		scanf("%d",&choose_num);
		while(getchar() != 10);

		switch(choose_num)
		{
		case LOGIN:
			//登录
			send(sfd,&choose_num,sizeof(choose_num),0);
			do_login(sfd);
			break;
		case QUIT:
			//断开连接
			send(sfd,&choose_num,sizeof(choose_num),0);
			close(sfd);
			exit(0);
			break;
		case CLEAR:
			//执行清屏
			system("clear");
			break;
		default:
			printf("*****输入指令错误*****\n");
			break;

		}
	}
	return 0;
}


int do_login(int sfd)
{
	_USER user;
	char buf[BUF_SIZE]= "";
	ssize_t res = 0;
	int choose_num= 0;

	//获取用户名和密码
	printf("请输入用户名>>>");
	fgets(user.id,sizeof(user.id),stdin);
	user.id[strlen(user.id)-1] = '\0';
	printf("请输入密码>>>");
	fgets(user.password,sizeof(user.password),stdin);
	user.password[strlen(user.password)-1] = '\0';

	user_me = user;

	if(strcmp(user.id,ROOT) == 0)
		user.type = 1;
	else
		user.type = 0;

	//发送给服务器
	send(sfd,(void*)&user,sizeof(user),0);

	bzero(buf,sizeof(buf));

	//接收服务器发过来的文件内容
	res = recv(sfd,buf,sizeof(buf),0);

	if(strcmp(buf,"exists") == 0)
	{
		printf("*****该用户已登录*****\n");
		return -1;
	}

	else if(strcmp(buf,"password error") == 0)
	{
		printf("*****密码错误*****\n");
		return -1;
	}

	else if(strcmp(buf,"name error") == 0)
	{
		printf("*****用户不存在*****\n");
		return -1;
	}

	else if(strcmp(buf,"error") == 0)
	{
		printf("*****登陆异常*****\n");
		return -1;
	}

	else if(strcmp(buf,"success") == 0)
	{
		printf("*****登录成功！*****\n");
	}

	do_root_or_user_op(sfd,user);

}

int do_root_or_user_op(int sfd,_USER user)
{
	int choose_num= 0;
	while(1)
	{
		choose_num = 0;
		printf("\n******************************\n");
		printf("********1.返回上级菜单********\n");
		printf("********2.清        屏********\n");
		printf("********3.修改员工信息********\n");
		printf("********4.查询员工信息********\n");
		if(user.type == 1)
		{
			printf("********5.新 增  员 工********\n");
			printf("********6.删 除  员 工********\n");
		}
		printf("******************************\n");
		//获取指令
		scanf("%d",&choose_num);
		while(getchar() != 10);

		//判断指令并发送指令、执行指令
		switch(choose_num)
		{
		case USER_ADD:
			//新增
			if(user.type != 1)
			{
				printf("*****输入指令错误*****\n");
				break;
			}
			send(sfd,&choose_num,sizeof(choose_num),0); 
			do_user_add(sfd,user);
			break;
		case USER_DEL:
			//删除
			if(user.type != 1)
			{
				printf("*****输入指令错误*****\n");
				break;
			}
			send(sfd,&choose_num,sizeof(choose_num),0);
			do_user_del(sfd,user);
			break;
		case USER_MODIFY:
			//修改
			send(sfd,&choose_num,sizeof(choose_num),0);
			user_modify(sfd,user);
			break;
		case USER_SEARCH:
			//查询
			send(sfd,&choose_num,sizeof(choose_num),0);
			do_user_search(sfd,user);
			break;
		case QUIT:
			//返回上层
			send(sfd,&choose_num,sizeof(choose_num),0);
			return 0;
			break;
		case CLEAR:
			//清屏
			system("clear");
			break;
		default:
			printf("*****输入指令错误*****\n");
			break;
		}
	}


	return 0;
}

int do_user_add(int sfd,_USER user)
{
	char buf[BUF_SIZE]= "";
	ssize_t res = 0;
	_USER_MSG user_msg;
	memset(&user_msg,0 ,sizeof(user_msg));
	//获取用户名和密码
	printf("请输入工号>>>");
	fgets(user_msg.id,sizeof(user_msg.id),stdin);
	user_msg.id[strlen(user_msg.id)-1] = '\0';
	printf("请输入姓名>>>");
	fgets(user_msg.name,sizeof(user_msg.name),stdin);
	user_msg.name[strlen(user_msg.name)-1] = '\0';
	printf("请输入年龄>>>");
	fgets(user_msg.age,sizeof(user_msg.age),stdin);
	user_msg.age[strlen(user_msg.age)-1] = '\0';
	printf("请输入性别>>>");
	fgets(user_msg.sex,sizeof(user_msg.sex),stdin);
	user_msg.sex[strlen(user_msg.sex)-1] = '\0';
	printf("请输入部门>>>");
	fgets(user_msg.department,sizeof(user_msg.department),stdin);
	user_msg.department[strlen(user_msg.department)-1] = '\0';
	printf("请输入薪资>>>");
	fgets(user_msg.salary,sizeof(user_msg.salary),stdin);
	user_msg.salary[strlen(user_msg.salary)-1] = '\0';
	printf("请输入电话>>>");
	fgets(user_msg.phone,sizeof(user_msg.phone),stdin);
	user_msg.phone[strlen(user_msg.phone)-1] = '\0';
	printf("请输入地址>>>");
	fgets(user_msg.address,sizeof(user_msg.address),stdin);
	user_msg.address[strlen(user_msg.address)-1] = '\0';

	//发送给服务器
	send(sfd,(void*)&user_msg,sizeof(user_msg),0);

	bzero(buf,sizeof(buf));

	//接收服务器发过来的文件内容
	res = recv(sfd,buf,sizeof(buf),0);
	if(res < 0)
	{
		ERR_MSG("recv");
		return -1;
	}
	else if(res == 0)
	{
		printf("*****服务器关闭*****\n");
		return 0;
	}
	//如果用户名已存在，服务器发过来一个错误信息
	if(strcmp(buf,"exists") == 0)
	{
		printf("*****注册失败：用户名已存在*****\n");
		return -1;
	}

	printf("*****注册成功！*****\n");
	return 0;
}

int do_user_del(int sfd,_USER user)
{
	char buf[BUF_SIZE]= "";
	ssize_t res = 0;
	_USER_MSG user_msg;
	//获取工号
	printf("请输入工号>>>");
	fgets(user_msg.id,sizeof(user_msg.id),stdin);
	user_msg.id[strlen(user_msg.id)-1] = '\0';

	//发送给服务器
	send(sfd,(void*)&user_msg,sizeof(user_msg),0);

	bzero(buf,sizeof(buf));

	//接收服务器发过来的文件内容
	res = recv(sfd,buf,sizeof(buf),0);
	if(res < 0)
	{
		ERR_MSG("recv");
		return -1;
	}
	else if(res == 0)
	{
		printf("*****服务器关闭*****\n");
		return 0;
	}
	//如果用户名不存在，服务器发过来一个错误信息
	if(strcmp(buf,"none") == 0)
	{
		printf("*****删除失败：用户名不存在*****\n");
		return -1;
	}

	printf("*****删除成功！*****\n");
	return 0;


}

int do_user_search(int sfd,_USER user)
{
	char buf[BUF_SIZE]= "";
	ssize_t res = 0;
	_USER_MSG user_msg;
	memset(&user_msg,0 ,sizeof(user_msg));
	//获取工号
	if(user.type == 1)

		printf("请输入工号或姓名(查询所有员工[*]）>>>");
	else
		printf("请输入工号或姓名>>>");
	fgets(user_msg.id,sizeof(user_msg.id),stdin);
	user_msg.id[strlen(user_msg.id)-1] = '\0';

	//发送给服务器
	send(sfd,(void*)&user_msg,sizeof(user_msg),0);

	puts("--------------------------------------------------------------------------------\n");
	if(user.type == 1)
	{
		printf("  部门      工号    姓名    年龄  性别  工资      电话         住址       \n" );
	}
	else
	{
		if(strcmp(user.id,user_msg.id) == 0)
		{
			printf("  部门      工号    姓名    年龄  性别  工资      电话         住址       \n" );
		}
		else
		{
			printf("  部门      工号      姓名      电话    \n");

		}
	}

	puts("--------------------------------------------------------------------------------\n");
	while(1)
	{
		memset(&user_msg,0 ,sizeof(user_msg));
		//接收服务器发过来的文件内容
		res = recv(sfd,(void*)&user_msg,sizeof(user_msg),0);
		if(res < 0)
		{
			ERR_MSG("recv");
			return -1;
		}
		else if(res == 0)
		{
			printf("*****服务器关闭*****\n");
			return 0;
		}
		//如果用户名不存在，服务器发过来一个错误信息
		if(strcmp(user_msg.id,"empty") == 0)
		{
			printf("*****查询失败：用户不存在*****\n");
			break;
		}
		if(strcmp(user_msg.id,"success") == 0)
		{
			printf("*****OVER*****\n");
			break;
		}

		if(user.type == 1)
		{
			printf("  %-12s%-8s%-12s%-6s%-6s%-10s%-14s%-14s\n" ,user_msg.department,user_msg.id,user_msg.name,\
					user_msg.age,user_msg.sex,user_msg.salary,user_msg.phone,user_msg.address); 
		}
		else
		{
			if(strcmp(user.id,user_msg.id) == 0)
			{
				printf("  %-12s%-8s%-12s%-6s%-6s%-10s%-14s%-14s\n" ,user_msg.department,user_msg.id,user_msg.name,\
						user_msg.age,user_msg.sex,user_msg.salary,user_msg.phone,user_msg.address); 
			}
			else
			{
				printf("  %-10s%-10s%-10s%-14s\n",user_msg.department,user_msg.id,user_msg.name,user_msg.phone); 

			}
		}
		puts("--------------------------------------------------------------------------------\n");

	}
	return 0;
}



int user_modify(int sfd,_USER user)
{
	int choose_num = 0;
	_USER_MSG user_msg;
	memset(&user_msg,0 ,sizeof(user_msg));

	while(1)
	{
		choose_num = 0;
		printf("\n******************************\n");
		printf("********1.返回上级菜单********\n");
		printf("********2.清        屏********\n");
		printf("********3.修改密码************\n");
		printf("********4.修改电话************\n");
		printf("********5.修改住址************\n");
		if(user.type == 1)
		{
			printf("********6.修改部门************\n");
			printf("********7.修改薪资************\n");
		}
		printf("******************************\n");
		//获取指令
		scanf("%d",&choose_num);
		while(getchar() != 10);

		switch(choose_num)
		{
		case MODIFY_PASSWORD:
			//修改密码
			send(sfd,&choose_num,sizeof(choose_num),0);
			do_modify_password(sfd,user);
			break;
		case MODIFY_PHONE:
			//修改电话
			send(sfd,&choose_num,sizeof(choose_num),0);
			do_modify_msg(sfd,user,choose_num);
			break;
		case MODIFY_ADDRESS:
			//修改住址
			send(sfd,&choose_num,sizeof(choose_num),0);
			do_modify_msg(sfd,user,choose_num);
			break;
		case MODIFY_DEPARTMENT:
			//修改部门
			if(user.type != 1)
			{
				printf("*****输入指令错误*****\n");
				break;
			}
			send(sfd,&choose_num,sizeof(choose_num),0);
			do_modify_msg(sfd,user,choose_num);
			break;
		case MODIFY_SALARY:
			//修改薪资
			if(user.type != 1)
			{
				printf("*****输入指令错误*****\n");
				break;
			}
			send(sfd,&choose_num,sizeof(choose_num),0);
			do_modify_msg(sfd,user,choose_num);
			break;
		case QUIT:
			//返回上级菜单
			send(sfd,&choose_num,sizeof(choose_num),0);
			return 0;
			break;
		case CLEAR:
			system("clear");
			break;
		default:
			printf("*****输入指令错误*****\n");
			break;
		}
	}
	return 0;
}

int do_modify_password(int sfd,_USER user)
{
	char buf[BUF_SIZE]= "";
	ssize_t res = 0;
	if(user.type == 1)
	{
		//获取工号
		printf("请输入员工工号>>>");
		fgets(user.id,sizeof(user.id),stdin);
		user.id[strlen(user.id)-1] = '\0';

		if(strcmp(user.id,ROOT) != 0)
		{
			strcpy(user.password,"123456");
			printf("正在重置密码为[123456]\n");

		}
		else
		{
			printf("请输入旧密码>>>");
			fgets(buf,sizeof(buf),stdin);
			buf[strlen(buf)-1] = '\0';

			if(strcmp(user_me.password,buf) != 0)
			{
				printf("密码错误！\n");
				return -1;
			}
			else{
				printf("请输入新密码>>>");
				fgets(user.password,sizeof(user.password),stdin);
				user.password[strlen(user.password)-1] = '\0';

			}
		}
	}
	else{
		printf("请输入旧密码>>>");
		fgets(buf,sizeof(buf),stdin);
		buf[strlen(buf)-1] = '\0';
		if(strcmp(user_me.password,buf) != 0)
		{
			printf("密码错误！\n");
			return -1;
		}
		else{
			printf("请输入新密码>>>");
			fgets(user.password,sizeof(user.password),stdin);
			user.password[strlen(user.password)-1] = '\0';

		}
	}

	send(sfd,(void*)&user,sizeof(user),0);

	res = recv(sfd, buf,sizeof(buf),0);
	if(res < 0)
	{
		ERR_MSG("recv");
		return -1;
	}
	else if(res == 0)
	{
		printf("*****服务器关闭*****\n");
		return 0;
	}
	//如果用户名不存在，服务器发过来一个错误信息
	if(strcmp(buf,"none") == 0)
	{
		printf("*****修改失败：用户不存在*****\n");
		return -1;
	}
	if(strcmp(buf,"success") == 0)
	{
		printf("密码修改成功\n");
	}
	if(user.type == 1 && strcmp(user.id ,ROOT) != 0)
		return 0;
	strcpy(user_me.password , user.password);

	return 0;
}


int do_modify_msg(int sfd,_USER user,int choose_num)
{
	char buf[BUF_SIZE]= "";
	ssize_t res = 0;
	_USER_MSG user_msg;
	strcpy(user_msg.id , user.id);

	printf("id =  %s __%d__\n",user_msg.id,__LINE__);
	printf("password =  %s __%d__\n",user_msg.phone,__LINE__);
	if(user.type == 1)
	{
		//获取工号
		printf("请输入员工工号>>>");
		fgets(user_msg.id,sizeof(user_msg.id),stdin);
		user_msg.id[strlen(user_msg.id)-1] = '\0';
	}
	if(choose_num == MODIFY_PHONE)
	{
		printf("请输入新电话号码>>>");
		strcpy(user_msg.update_key,"phone");
	}
	else if(choose_num == MODIFY_ADDRESS)
	{
		printf("请输入新住址>>>");
		strcpy(user_msg.update_key,"address");
	}
	else if(choose_num == MODIFY_DEPARTMENT)
	{
		printf("请输入新部门>>>");
		strcpy(user_msg.update_key,"department");
	}
	else if(choose_num == MODIFY_SALARY)
	{
		printf("请输入新工资>>>");
		strcpy(user_msg.update_key,"salary");
	}

	fgets(user_msg.update_msg,sizeof(user_msg.update_msg),stdin);
	user_msg.update_msg[strlen(user_msg.update_msg)-1] = '\0';

	send(sfd,(void*)&user_msg,sizeof(user_msg),0);

	res = recv(sfd, buf,sizeof(buf),0);
	if(res < 0)
	{
		ERR_MSG("recv");
		return -1;
	}
	else if(res == 0)
	{
		printf("*****服务器关闭*****\n");
		return 0;
	}
	//如果用户名不存在，服务器发过来一个错误信息
	if(strcmp(buf,"none") == 0)
	{
		printf("*****修改失败：用户不存在*****\n");
		return -1;
	}
	if(strcmp(buf,"success") == 0)
	{
		printf("信息修改成功\n");
	}

	return 0;
}





