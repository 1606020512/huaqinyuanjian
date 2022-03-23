all:server client
server: Tcp_server_func.c Tcp_server.c
	gcc Tcp_server_func.c Tcp_server.c -o server -lpthread -lsqlite3

client: Tcp_client.c  Tcp_client_func.c
	gcc Tcp_client.c  Tcp_client_func.c -o client -lpthread -lsqlite3
clean:
	rm server client

