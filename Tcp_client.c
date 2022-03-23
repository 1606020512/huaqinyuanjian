#include "Tcp.h"
int main(int argc, const char *argv[])
{
	//创建tcp客户端
	int cfd = client_create();

	//客户端初始菜单函数
	client_initial_menu(cfd);

	//销毁描述符函数
	close(cfd);
	return 0;
}
