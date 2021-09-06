#include "global.h"
#include "link_list.h"

#define LEN_ENV 20
#define LEN_RFID 4 
extern int dev_uart_fd;  //M0设备节点文件描述符
extern linklist linkHead; //头结点
extern pthread_cond_t cond_analysis;
extern pthread_mutex_t mutex_linklist;

void serial_init(int fd)
	{
	struct termios options;
	tcgetattr(fd,&options);   //读取终端参数
	options.c_cflag |= (CLOCAL | CREAD);  //忽略调制解调器线路状态，使用接收器
	options.c_cflag &= ~CSIZE;    //清除目前字符长度
	options.c_cflag &= ~CRTSCTS;   //不使用RTS/CTS控制流
	options.c_cflag |= CS8;     //字符长度设置为8
	options.c_cflag &= ~CSTOPB;   //设置1各标志位
	options.c_iflag |= IGNPAR;    //允许输入奇偶校验
	options.c_iflag &= ~(ICRNL | IXON);   //回车不转为换行。不允许输入时对XON/XOFF流进行控制
	options.c_oflag = 0; 
	options.c_lflag = 0;

	cfsetispeed(&options,B115200);  //设置波特率为115200
	cfsetospeed(&options,B115200);
	tcsetattr(fd,TCSANOW,&options);   //设置终端参数
	}


void *pthread_transfer(void *arg)
{
	printf("this is pthread_transfer\n");
	int i = 0, len;
	char flag = 0, check;
	link_datatype buf; //数据节点
	linkHead = CreateEmptyLinklist();
	if((dev_uart_fd = open(DEV_ZIGBEE,O_RDWR)) < 0)
	{
		ERR_MSG("open ttyUSB0");
		exit(-1);
	}
	
	serial_init(dev_uart_fd);
	printf("pthread_transfer is ok\n");

	while(1)
	{
		memset(&buf,0,sizeof(link_datatype));
		read(dev_uart_fd,&check,1);
		if(check == 's')
		{
			check = 0;
			read(dev_uart_fd,&check,1);
			if(check == 't')
			{
				check = 0;
				read(dev_uart_fd,&check,1);
				if(check == ':')
				{
					check = 0;
					read(dev_uart_fd,&check,1);
					if(check == 'e')
					{
						buf.msg_type = 'e';
						usleep(1);
						if((len = read(dev_uart_fd,buf.text,LEN_ENV)) != LEN_ENV)
						{
							for(i = len; i < LEN_ENV; i++)
							{
								read(dev_uart_fd,buf.text+i,1);
							}
						}
						flag = 1;
					}
					else if(check == 'r')
					{
						buf.msg_type = 'r';
						usleep(1);
						if((len = read(dev_uart_fd,buf.text,LEN_RFID)) != LEN_RFID)
						{
							for(i = len; i < LEN_RFID; i++)
							{
								read(dev_uart_fd,buf.text+i,1);
							}
						}
						flag = 1;

					}
				}

			}

		}
		if(1 == flag)
		{
			pthread_mutex_lock(&mutex_linklist);
			if((InsertLinknode(buf)) == -1)
			{
				pthread_mutex_unlock(&mutex_linklist);
				printf("NONMEM\n");
			}
			pthread_mutex_unlock(&mutex_linklist);
			flag = 0;
			pthread_cond_signal(&cond_analysis);
		}
	}
	pthread_exit(NULL);
}

