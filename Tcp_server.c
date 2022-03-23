#include "Tcp.h"
void* rcv_cli_info(void* arg)
{
	//分离线程
	pthread_detach(pthread_self());

	int newfd = (*(struct sig*)arg).sig_fd;
	sqlite3* db = (*(struct sig*)arg).sig_db;

	server_initial_menu(newfd,db);

	while(1)
	{
	
	/*	strcat(buf, "*_*");
		//发送数据
		if(send(newfd, buf, sizeof(buf), 0) < 0)
		{
			ERR_MSG("send");
			break;
		}
		printf("发送成功\n");
	*/
		}

	close(newfd);

	pthread_exit(NULL);

}
int main(int argc, const char *argv[])
{
	char rbuf[128],sbuf[128];
	//创建员工个人信息表
	sqlite3* db = sqlite3_creat();
	
	//创建tcp服务器
	int sfd =  server_create();
	if(sfd < 0)
	{
		printf("创建tcp服务器失败\n");
		return -1;
	}
	struct sockaddr_in cin;
	socklen_t len = sizeof(cin);
	pthread_t tid;
	


	while(1)
	{ 	//主线程负责连接
		int newfd = accept(sfd,(struct sockaddr*)&cin,&len);
		if(0 > newfd)
		{
			ERR_MSG("accept");
			return -1;
		}

		struct sig signal;
		signal.sig_fd = newfd;
		signal.sig_db = db;
		//创建线程负责处理信息
		if(pthread_create(&tid, NULL, rcv_cli_info, (void*)&signal) != 0)
		{
			ERR_MSG("pthread_create");
			return -1;
		}	


	}
	sqlite3_close(db);
	close(sfd);
	return 0;
}
