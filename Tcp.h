#ifndef __Tcp_H__
#define __Tcp_H__
#include <sqlite3.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#define ERR_MSG(msg) 	do{\
	fprintf(stderr,"__%d__",__LINE__);\
	perror(msg);\
}while(0)

struct sig{
	int sig_fd;
	sqlite3* sig_db;
};

#define IP "192.168.1.56"
#define PORT  1025


int client_create();
int client_register(int cfd);
void client_revise(int cfd);
void client_ordinary(char* id,int cfd);
void client_administrator(int cfd);
int client_log_in(int cfd);
void client_initial_menu(int cfd);

sqlite3* sqlite3_creat();
int server_register(int newfd,sqlite3*db);
char *server_search(int newfd,sqlite3*db);
void server_revise(int newfd,sqlite3* db);
void server_delete(int newfd,sqlite3*db);
void server_control(int newfd,sqlite3*db);
void server_load_in(int newfd,sqlite3* db);
int server_initial_menu(int newfd,sqlite3*db);
int server_create();
#endif
