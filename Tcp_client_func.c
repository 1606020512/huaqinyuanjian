#include "Tcp.h"
//创建tcp客户端
int client_create()
{

	char rbuf[128],sbuf[128];
	int cfd = socket(AF_INET,SOCK_STREAM,0);
	if(0 > cfd)
	{
		ERR_MSG("socket");
		return -1;
	}
	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port   = htons(PORT);
	sin.sin_addr.s_addr = inet_addr(IP);

	if(0 > connect(cfd,(struct sockaddr*)&sin,sizeof(sin)))
	{
		ERR_MSG("connect");
		return -1;
	}

	return cfd;
}
//客户端注册函数
int client_register(int cfd)
{
	char sbuf[128],rbuf[128];
	char name[128],addr[128],department[128];
	int id,old,key,salary;
	int r,k=0;
	long int phone;

	while(1)
	{
		puts(">>>>>输入1普通用户注册<<<<<");
		puts(">>>>>输入2管理用户登录<<<<<");
		puts(">>>>>>输入3返回上一级<<<<<<");
		scanf(" %s",sbuf);
		int s = send(cfd,sbuf,sizeof(sbuf),0);
		if(0 >= s)
		{
			ERR_MSG("send");
			return -1;
		}
		switch(sbuf[0])
		{
		case '1':
			puts("请依次输入要注册的账号和密码");
			scanf("%d %d",&id,&key);

			sprintf(sbuf,"(%d,%d)",id,key);
			s = send(cfd,sbuf,sizeof(sbuf),0);
			if(0 >s)
			{
				ERR_MSG("send");
				return -1;
			}
			bzero(rbuf,sizeof(rbuf));
			r = recv(cfd,rbuf,sizeof(rbuf),0);
			if(0 > r)	
			{
				ERR_MSG("recv");
				return -1;
			}
			else if(0 == r)
			{
				printf("服务端已关闭\n");
				return -1;
			}
			if(rbuf[0] == '1')
			{
				puts("输入的账号重复,请重新输入");
				break;
			}
			puts("请依次输入你的工号名字年龄住址薪资电话部门");
			scanf("%d %s %d %s %d %ld %s",&id,name,&old,addr,&salary,&phone,department);
			memset(sbuf,0,sizeof(sbuf));
			sprintf(sbuf,"(%d,\"%s\",%d,\"%s\",%d,%ld,\"%s\")",id,name,old,addr,salary,phone,department);
			s = send(cfd,sbuf,sizeof(sbuf),0);
			if(0 >s)
			{
				ERR_MSG("send");
				return -1;
			}
			break;
		case '2':
			puts("请输入管理员秘钥");
			scanf("%d",&k);
			if(k == 1)
			{
				puts("请依次输入要注册的账号和密码");
				scanf("%d %d",&id,&key);

				sprintf(sbuf,"(%d,%d)",id,key);
				s = send(cfd,sbuf,sizeof(sbuf),0);
				if(0 >s)
				{
					ERR_MSG("send");
					return -1;
				}
				r = recv(cfd,rbuf,sizeof(rbuf),0);
				if(0 > r)	
				{
					ERR_MSG("recv");
					return -1;
				}
				else if(0 == r)
				{
					printf("服务端已关闭\n");
					return -1;
				}
				if(rbuf[0] == '1')
				{
					puts("输入的账号重复,请重新输入");
					break;
				}
				puts("请依次输入你的工号名字年龄住址薪资电话部门");
				scanf("%d %s %d %s %d %ld %s",&id,name,&old,addr,&salary,&phone,department);
				memset(sbuf,0,sizeof(sbuf));
				sprintf(sbuf,"(%d,\"%s\",%d,\"%s\",%d,%ld,\"%s\")",id,name,old,addr,salary,phone,department);
				s = send(cfd,sbuf,sizeof(sbuf),0);
				if(0 >s)
				{
					ERR_MSG("send");
					return -1;
				}
			}
			else
				puts("秘钥错误了,秘钥是1");
				break;
		case '3':
			return 0;
		}
	}
	return 0;

}
//修改员工信息函数
void client_revise(int cfd)
{
	char sbuf[128],rbuf[128];
	char name[128],addr[128],department[128];
	int id,old,key,salary;
	int r,s;
	long int phone;

	puts("请依次输入你的工号名字年龄住址薪资电话部门");
	scanf("%d %s %d %s %d %ld %s",&id,name,&old,addr,&salary,&phone,department);
	memset(sbuf,0,sizeof(sbuf));
	sprintf(sbuf,"(%d,\"%s\",%d,\"%s\",%d,%ld,\"%s\")",id,name,old,addr,salary,phone,department);
	s = send(cfd,sbuf,sizeof(sbuf),0);
	if(0 >s)
	{
		ERR_MSG("send");
		exit(0);
	}
	return;
}

//普通员工客户端登录菜单函数
void client_ordinary(char* id,int cfd)
{
	int node;
	char rbuf[128]="1",sbuf[128];
	int r,i,s;
	while(1)
	{
		puts(">>>>>输入s查询员工信息<<<<<");
		puts(">>>>>输入r修改员工信息<<<<<");
		puts(">>>>>输入q退出登录菜单<<<<<");	
		bzero(sbuf,sizeof(sbuf));
		scanf("%s",sbuf);
		s = send(cfd,sbuf,sizeof(sbuf),0);
		if(0 > s)
		{
			ERR_MSG("send");
			exit(0);
		}

		switch(sbuf[0])
		{
			//查找员工信息函数
		case 's':
			puts("请输入要查找的员工工号");
			bzero(sbuf,sizeof(sbuf));
			scanf("%s",sbuf);
			s = send(cfd,sbuf,sizeof(sbuf),0);
			if(0 >s)
			{
				ERR_MSG("send");
				exit(0);
			}
			puts("   id    name    old  addr  salary  phone  department");
			i=0;
			while(i!=7)
			{
				bzero(rbuf,sizeof(rbuf));		
				r = recv(cfd,rbuf,sizeof(rbuf),0);
				if(0 >r)
				{
					ERR_MSG("recv");
					exit(0);
				}else if(0 == r)
				{
					printf("客户端已关闭\n");
				}
				if(!strcmp(rbuf,"数据库中没有该成员"))
				{
					break;
				}
				if(i==2||i==3||i==4)
				{
					strcpy(rbuf,"***");
				}
				printf(" %5s ",rbuf);
				if(i==6)
					putchar('\n');
				i++;
			}
			break;

			//修改员工信息函数
		case 'r':
			puts("请输入要修改的员工工号");
			bzero(sbuf,sizeof(sbuf));
			scanf("%s",sbuf);
			puts(id);
			puts(sbuf);
			if(strcmp(id,sbuf))
			{
				puts("非管理员无权修改其他人信息  仅能修改自己");
				break;
			}
			s = send(cfd,sbuf,sizeof(sbuf),0);
			if(0 >s)
			{
				ERR_MSG("send");
				exit(0);
			}
			puts("   id    name    old  addr  salary  phone  department");
			i=0;
			while(i!=7)
			{
				bzero(rbuf,sizeof(rbuf));		
				r = recv(cfd,rbuf,sizeof(rbuf),0);
				if(0 >r)
				{
					ERR_MSG("recv");
					exit(0);
				}else if(0 == r)
				{
					printf("客户端已关闭\n");
				}
				if(!strcmp(rbuf,"数据库中没有该成员"))
				{
					break;
				}
				printf(" %5s ",rbuf);
				if(i==6)
					putchar('\n');
				i++;
			}
			client_revise(cfd);

			break;
			//退出登录菜单函数
		case 'q':
			return;
		}

	}
}

//管理员工客户端登录菜单函数
void client_administrator(int cfd)
{
	int node;
	char rbuf[128]="1",sbuf[128];
	char key[128]; 
	int r,i;

	while(1)
	{
		puts(">>>>>输入s查询员工信息<<<<<");
		puts(">>>>>输入r修改员工信息<<<<<");
		puts(">>>>>输入d删除员工信息<<<<<");	
		puts(">>>>>输入i增加员工信息<<<<<");	
		puts(">>>>>输入q退出登录菜单<<<<<");	
		bzero(sbuf,sizeof(sbuf));
		scanf("%s",sbuf);
		int s = send(cfd,sbuf,sizeof(sbuf),0);
		if(0 > s)
		{
			ERR_MSG("send");
			exit(0);
		}

		switch(sbuf[0])
		{
			//查找员工信息函数

		case 's':				
			puts("请输入要查找的员工工号");
			bzero(sbuf,sizeof(sbuf));
			scanf("%s",sbuf);
			s = send(cfd,sbuf,sizeof(sbuf),0);
			if(0 >s)
			{
				ERR_MSG("send");
				exit(0);
			}
			puts("   id    name    old  addr  salary  phone  department");
			i=0;
			while(i!=7)
			{
				bzero(rbuf,sizeof(rbuf));		
				r = recv(cfd,rbuf,sizeof(rbuf),0);
				if(0 >r)
				{
					ERR_MSG("recv");
					exit(0);
				}else if(0 == r)
				{
					printf("客户端已关闭\n");
				}
				if(!strcmp(rbuf,"数据库中没有该成员"))
				{
					break;
				}
				printf(" %5s ",rbuf);
				if(i==6)
					putchar('\n');
				i++;
			}
			break;

			//修改员工信息函数
		case 'r':
			puts("请输入要修改的员工工号");

			bzero(sbuf,sizeof(sbuf));
			scanf("%s",sbuf);
			s = send(cfd,sbuf,sizeof(sbuf),0);
			if(0 >s)
			{
				ERR_MSG("send");
				exit(0);
			}
			puts("   id    name    old  addr  salary  phone  department");
			i=0;
			while(i!=7)
			{
				bzero(rbuf,sizeof(rbuf));		
				r = recv(cfd,rbuf,sizeof(rbuf),0);
				if(0 >r)
				{
					ERR_MSG("recv");
					exit(0);
				}else if(0 == r)
				{
					printf("客户端已关闭\n");
				}
				if(!strcmp(rbuf,"数据库中没有该成员"))
				{
					break;
				}
				printf(" %5s ",rbuf);
				if(i==6)
					putchar('\n');
				i++;
			}
			client_revise(cfd);
			break;
			//退出登录菜单函数
		case 'q':
			return;
			//删除员工信息函数
		case 'd':
			puts("请输入要删除的员工工号");
			bzero(sbuf,sizeof(sbuf));
			scanf("%s",sbuf);
			s = send(cfd,sbuf,sizeof(sbuf),0);
			if(0 >s)
			{
				ERR_MSG("send");
				exit(0);
			}
			puts("   id    name    old  addr  salary  phone  department");
			i=0;
			while(i!=7)
			{
				bzero(rbuf,sizeof(rbuf));		
				r = recv(cfd,rbuf,sizeof(rbuf),0);
				if(0 >r)
				{
					ERR_MSG("recv");
					exit(0);
				}else if(0 == r)
				{
					printf("客户端已关闭\n");
				}
				if(!strcmp(rbuf,"数据库中没有该成员"))
				{
					puts(rbuf);
					goto A;
				}
				printf(" %5s ",rbuf);
				if(i==6)
					putchar('\n');
				i++;
			}
			puts("是否确认删除(Y/N)");
			bzero(sbuf,sizeof(sbuf));
			scanf("%s",sbuf);

			s = send(cfd,sbuf,sizeof(sbuf),0);
			if(0 >s)
			{
				ERR_MSG("send");
				exit(0);
			}


A:			break;
			//增加员工信息函数
		case 'i':
			client_register(cfd);
			break;
		}

	}
}

//客户端登录函数
int client_log_in(int cfd)
{

	char rbuf[128],sbuf[128];
	char key[128];
	puts("请输入账号密码");
	scanf("%s %s",sbuf,key);
	char tt[128];
	strcpy(tt,sbuf);

	int s = send(cfd,sbuf,sizeof(sbuf),0);
	if(0 > s)
	{
		ERR_MSG("send");
		return -1;
	}
	int r = recv(cfd,rbuf,sizeof(rbuf),0);
	if(0 > r)	
	{
		ERR_MSG("recv");
		return -1;
	}
	else if(0 == r)
	{
		printf("服务端已关闭\n");
		return -1;
	}
	if(strcmp(rbuf,key))
	{
		puts("账号或密码错误");
		bzero(sbuf,sizeof(sbuf));
		strcpy(sbuf,"0");
		s = send(cfd,sbuf,sizeof(sbuf),0);
		if(0 > s)
		{
			ERR_MSG("send");
			return -1;
		}
	}
	else
	{
		bzero(sbuf,sizeof(sbuf));
		strcpy(sbuf,"1");
		s = send(cfd,sbuf,sizeof(sbuf),0);
		if(0 > s)
		{
			ERR_MSG("send");
			return -1;
		}
		r = recv(cfd,rbuf,sizeof(rbuf),0);
		if(0 > r)	
		{
			ERR_MSG("recv");
			return -1;
		}
		else if(0 == r)
		{
			printf("服务端已关闭\n");
			return -1;
		}
		switch(rbuf[0])
		{
		case '0':client_ordinary(tt,cfd);
				 break;
		case '1':client_administrator(cfd);
				 break;


		}
	}
}
//客户端初始菜单函数
void client_initial_menu(int cfd)
{
	char rbuf[128],sbuf[128];
	int node;

	while(1)
	{
		puts(">>>>>输入1注册<<<<<");
		puts(">>>>>输入2登录<<<<<");
		puts(">>>>>输入3退出<<<<<");	
		scanf("%s",sbuf);
		int s = send(cfd,sbuf,sizeof(sbuf),0);
		if(0 >= s)
		{
			ERR_MSG("send");
			exit(0);
		}

		switch(sbuf[0])
		{
			//注册函数
		case '1':client_register(cfd);
				 break;
				 //登录函数
		case '2':client_log_in(cfd);
				 break;
				 //退出函数
		case '3':
				 return;

		}
	}
	return ;
}
