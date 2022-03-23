#include "Tcp.h"
//tcp服务器创建员工个人信息表函数
sqlite3* sqlite3_creat()
{
	//打开数据库
	sqlite3* db = NULL;
	if(sqlite3_open("./my.db", &db) != SQLITE_OK)
	{
		printf("sqlite3_open failed\n");
		printf("%d\n", sqlite3_errcode(db));
		fprintf(stderr, "__%d__ sqlite3_open: %s\n", __LINE__,sqlite3_errmsg(db));
		return NULL;
	}
	//创建员工个人信息表
	char sql[128] = "create table if not exists member_list (id int primary key, name char, old int,addr char,salary int,phone int,department char)";
	char* errmsg = NULL;
	if(sqlite3_exec(db, sql, NULL, NULL, &errmsg) != 0)
	{
		fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg);
		return NULL;
	}
	memset(sql,0,sizeof(sql));
	strcpy(sql,"create table if not exists load_list (id int primary key,key int,administrator int)");
	if(sqlite3_exec(db, sql, NULL, NULL, &errmsg) != 0)
	{
		fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg);
		return NULL;
	}
	return db;
}
//tcp服务器初始界面注册函数
int server_register(int newfd,sqlite3*db)
{
	char buf[128] = "";
	ssize_t res = 0;
	char* errmsg = NULL;
	char sql[128] = "insert into load_list values(id int primary key, name char, old int,addr char,salary int,phone int,department char)";
	while(1)
	{
		bzero(buf, sizeof(buf));
		res = recv(newfd, buf, sizeof(buf), 0);
		if(res < 0)
		{
			ERR_MSG("recv");
			return -1;
		}
		else if(0 == res)
		{
			printf("客户端断开连接\n"); 
			return -1;
		}
		puts(buf);
		switch(buf[0])
		{
		case '1':
			bzero(buf, sizeof(buf));
			res = recv(newfd, buf, sizeof(buf), 0);
			buf[strlen(buf)-1]=0;
			strcat(buf,",0)");

			sprintf(sql,"insert into load_list values %s",buf);	
			if(sqlite3_exec(db, sql, NULL, NULL, &errmsg) != 0)
			{
				bzero(buf, sizeof(buf));
				strcpy(buf,"1");
				send(newfd,buf,sizeof(buf),0);
				break;
			}
			//验证账号重复性
			bzero(buf, sizeof(buf));
			strcpy(buf,"0");
			send(newfd,buf,sizeof(buf),0);

			bzero(buf, sizeof(buf));
			res = recv(newfd, buf, sizeof(buf), 0);
			sprintf(sql,"insert into member_list values %s",buf);	
			if(sqlite3_exec(db, sql, NULL, NULL, &errmsg) != 0)
			{
				fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg);
				return -1;
			}

			break;
		case '2':	
			bzero(buf, sizeof(buf));
			res = recv(newfd, buf, sizeof(buf), 0);
			buf[strlen(buf)-1]=0;
			strcat(buf,",1)");

			sprintf(sql,"insert into load_list values %s",buf);	
			if(sqlite3_exec(db, sql, NULL, NULL, &errmsg) != 0)
			{
				bzero(buf, sizeof(buf));
				strcpy(buf,"1");
				send(newfd,buf,sizeof(buf),0);
				break;
			}
			//验证账号重复性
			bzero(buf, sizeof(buf));
			strcpy(buf,"0");
			send(newfd,buf,sizeof(buf),0);

			bzero(buf, sizeof(buf));
			res = recv(newfd, buf, sizeof(buf), 0);
			sprintf(sql,"insert into member_list values %s",buf);	
			if(sqlite3_exec(db, sql, NULL, NULL, &errmsg) != 0)
			{
				fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg);
				return -1;
			}
			break;
		case '3':
			return 0;
		}
	}
}
//tcp服务器查找函数
char *server_search(int newfd,sqlite3*db)
{
	char rbuf[128],sbuf[128];
	char sql[512] = "select * from member_list where id=";
	char **pres = NULL;
	int row, column;
	char* errmsg = NULL;
	int i = 0;
	int r,s;
	static char aa[128];
	bzero(rbuf,sizeof(rbuf));
	r = recv(newfd,rbuf,sizeof(rbuf),0);
	if(0 > r)	
	{
		ERR_MSG("recv");
		exit(0);
	}
	else if(0 == r)
	{
		printf("服务端已关闭\n");
		exit(0);
	}
	strcat(sql,rbuf);
	strcpy(aa,rbuf);

	if(sqlite3_get_table(db, sql, &pres, &row, &column, &errmsg) != SQLITE_OK)
	{
		fprintf(stderr, "__%d__ sqlite3_get_table:%s\n", __LINE__, errmsg);
		exit(0);
	}
	if((row+1)*column < 7)
	{
		bzero(sbuf,sizeof(sbuf));
		strcpy(sbuf,"数据库中没有该成员");
		s = send(newfd,sbuf,sizeof(sbuf),0);
		if(0 > s)
		{
			ERR_MSG("send");
			return NULL;
		}
		return aa;
	}
	else
		for(i=7 ;i<(row+1)*column; i++)
		{
			printf("%10s ", pres[i]);
			if((i+1)%7 == 0)
			{
				putchar(10);
			}
			strcpy(sbuf,pres[i]);
			s = send(newfd,sbuf,sizeof(sbuf),0);
			if(0 >s)
			{
				ERR_MSG("send");
				exit(0);
			}
			puts(sbuf);

		}
	return aa;
}
//tcp服务端修改函数
void server_revise(int newfd,sqlite3* db)
{	char rbuf[128],sbuf[128];
	char sql[512];
	char **pres = NULL;
	int row, column,r;
	char* errmsg = NULL;
	char*aa=NULL;
	aa = server_search(newfd,db);

	strcpy(rbuf,aa);
	sprintf(sql,"delete from member_list where id=%s",rbuf);	
	if(sqlite3_exec(db, sql, NULL, NULL, &errmsg) != 0)
	{
		fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg);
		exit(0);
	}


	bzero(rbuf, sizeof(rbuf));
	r = recv(newfd, rbuf, sizeof(rbuf), 0);
	sprintf(sql,"insert into member_list values %s",rbuf);	
	if(sqlite3_exec(db, sql, NULL, NULL, &errmsg) != 0)
	{
		fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg);
		exit(0);
	}
	return;
}
//tcp服务端删除操作
void server_delete(int newfd,sqlite3*db)
{
	char rbuf[128],sbuf[128];
	char sql[512];
	char **pres = NULL;
	int row, column,r;
	char* errmsg = NULL;
	char*aa=NULL;
	aa = server_search(newfd,db);

	bzero(rbuf, sizeof(rbuf));
	r = recv(newfd, rbuf, sizeof(rbuf), 0);

	if(rbuf[0] == 'Y')
	{
		strcpy(rbuf,aa);
		sprintf(sql,"delete from member_list where id=%s",rbuf);	
		if(sqlite3_exec(db, sql, NULL, NULL, &errmsg) != 0)
		{
			fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg);
			exit(0);
		}
		sprintf(sql,"delete from load_list where id=%s",rbuf);	
		if(sqlite3_exec(db, sql, NULL, NULL, &errmsg) != 0)
		{
			fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg);
			exit(0);
		}
	}
	return;

}
//tcp服务端操作函数
void server_control(int newfd,sqlite3*db)
{
	char rbuf[128],sbuf[128];
	char sql[512] = "select * from member_list where id=";
	char **pres = NULL;
	int row, column;
	char* errmsg = NULL;
	int i = 0;
	int r,s;

	while(1)
	{
		bzero(rbuf,sizeof(rbuf));
		r = recv(newfd,rbuf,sizeof(rbuf),0);
		if(0 > r)	
		{
			ERR_MSG("recv");
			exit(0);
		}
		else if(0 == r)
		{
			printf("服务端已关闭\n");
			exit(0);
		}
		switch(rbuf[0])
		{
		case 's':

			server_search(newfd,db);
			break;
		case 'r':
			server_revise(newfd,db);
			break;
		case 'q':
			return;
		case 'd':
			server_delete(newfd,db);
			break;
		case 'i':
			server_register(newfd,db);
			break;

		}
	}
	return ;
}
//tcp服务器登录函数
void server_load_in(int newfd,sqlite3* db)
{
	char sql[128];
	int s;
	strcpy(sql,"select * from load_list where id=");
	char **pres = NULL;
	int row, column;
	char* errmsg = NULL;
	char rbuf[128],sbuf[128];
	bzero(rbuf,sizeof(rbuf));
	int r;
	r= recv(newfd,rbuf,sizeof(rbuf),0);
	if(0 > r)	
	{
		ERR_MSG("recv");
		exit(0) ;
	}
	else if(0 == r)
	{
		printf("服务端已关闭\n");
		exit(0) ;
	}
	strcat(rbuf,";");
	strcat(sql,rbuf);
	if(sqlite3_get_table(db, sql, &pres, &row, &column, &errmsg) != SQLITE_OK)
	{
		fprintf(stderr, "__%d__ sqlite3_get_table:%s\n", __LINE__, errmsg);
		exit(0) ;
	}
	bzero(sbuf,sizeof(sbuf));
	strcpy(sbuf,pres[4]);
	s = send(newfd,sbuf,sizeof(sbuf),0);
	if(0 > s)
	{
		ERR_MSG("send");
		exit(0) ;
	}

	bzero(rbuf,sizeof(rbuf));
	r = recv(newfd,rbuf,sizeof(rbuf),0);
	if(0 > r)	
	{
		ERR_MSG("recv");
		exit(0) ;
	}
	else if(0 == r)
	{
		printf("服务端已关闭\n");
		exit(0) ;
	}
	if(rbuf[0]!='1')
	{
		return;
	}
	else 
	{
		bzero(sbuf,sizeof(sbuf));
		strcpy(sbuf,pres[5]);
		s = send(newfd,sbuf,sizeof(sbuf),0);
		if(0 > s)
		{
			ERR_MSG("send");
			exit(0) ;
		}
		server_control(newfd,db);
	}



	return;
}
//tcp服务器初始菜单函数
int server_initial_menu(int newfd,sqlite3*db)
{
	char buf[128] = "";
	ssize_t res = 0;
	while(1)
	{
		bzero(buf, sizeof(buf));
		//接收数据
		res = recv(newfd, buf, sizeof(buf), 0);
		if(res < 0)
		{
			ERR_MSG("recv");
			return -1;
		}
		else if(0 == res)
		{
			printf("客户端断开连接\n"); 
			return -1;
		}


		switch(buf[0])
		{
		case '1':
			server_register(newfd,db);
			break;
		case '2':
			server_load_in(newfd,db);
			break;
		case '3':
			break;
		}
	}
	return 0;
}


//tcp服务器创建
int server_create()
{

	char rbuf[128],sbuf[128];
	int sfd = socket(AF_INET,SOCK_STREAM,0);
	if(0 > sfd)
	{
		ERR_MSG("socket");
		return -1;
	}
	int reuse = 1; 	//允许 
	if(setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
	{
		ERR_MSG("setsockopt");
		return -1;
	}

	struct sockaddr_in sin={
		.sin_family 		= AF_INET,
		.sin_port   		= htons(PORT),
		.sin_addr.s_addr 	= inet_addr(IP),
	};

	if(0 > bind(sfd,(struct sockaddr*)&sin,sizeof(sin)))
	{
		ERR_MSG("bind");
		return -1;
	}

	if(0 > listen(sfd,10))
	{
		ERR_MSG("socket");
		return -1;
	}
	return sfd;
}
