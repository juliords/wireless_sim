/* -------------------------------------------------------------------------- */

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "udp.h"

#define LOCALHOST "127.0.0.1"
#define BUF_MAX 1024

/* -------------------------------------------------------------------------- */

static inline int ws_socket_create()
{
	int sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (-1 == sock)
	{
		printf("Error Creating Socket");
		exit(EXIT_FAILURE);
	}

	return sock;
}

/* -------------------------------------------------------------------------- */

static inline void ws_thread_create(void* (*func)(void*), void *data)
{
	pthread_t tid;
	pthread_create(&tid, NULL, func, data);
	pthread_detach(tid);
}

/* -------------------------------------------------------------------------- */

struct thread_recv_data
{
	ws_recv_cb callback;

	char addr[20];
	int port;

	char buf[BUF_MAX];
	int buf_l;
};

static void* thread_recv(void *p)
{
	struct thread_recv_data *data = (struct thread_recv_data*)p;
	data->callback(data->addr, data->port, data->buf_l, data->buf);

	free(p);
	return NULL;
}

/* -------------------------------------------------------------------------- */

struct thread_listen_data
{
	ws_recv_cb callback;
	int port;
};

static void* thread_listen(void *p)
{
	struct thread_listen_data *data = (struct thread_listen_data*)p;

	struct sockaddr_in sa; 
	socklen_t sa_len;
	char buf[BUF_MAX];
	ssize_t recsize;
	int sock = ws_socket_create();

	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = htonl(INADDR_ANY);
	sa.sin_port = htons(data->port);
	sa_len = sizeof(sa);

	if (-1 == bind(sock,(struct sockaddr *)&sa, sizeof(sa)))
	{
		perror("error bind failed");
		exit(EXIT_FAILURE);
	}

	getsockname(sock, (struct sockaddr *)&sa, &sa_len);
	data->port = ntohs(sa.sin_port);

	for (;;) 
	{
		struct thread_recv_data *recv_data;

		recsize = recvfrom(sock, (void *)buf, sizeof(buf), 0, (struct sockaddr *)&sa, &sa_len);
		if (recsize < 0)
			continue;

		recv_data           = malloc(sizeof(struct thread_recv_data));
		recv_data->callback = data->callback;
		recv_data->port     = ntohs(sa.sin_port);
		recv_data->buf_l    = (int)recsize;
		strcpy(recv_data->addr, inet_ntoa(sa.sin_addr));
		memcpy(recv_data->buf, buf, (int)recsize);

		ws_thread_create(thread_recv, recv_data);
	}

	free(p);
	return NULL;
}

int ws_listen(ws_recv_cb cb, int port)
{
	struct thread_listen_data *data;

	if(cb == NULL) 
		return -1;

	data = malloc(sizeof(struct thread_listen_data));
	data->callback = cb;
	data->port = port;

	ws_thread_create(thread_listen, data);

	usleep(10);
	return data->port;
}

/* -------------------------------------------------------------------------- */

void ws_send(char *addr, int port, char *buf, int buf_l)
{
	struct sockaddr_in sa;
	int sock;

	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = inet_addr(addr);
	sa.sin_port = htons(port);

	sock = ws_socket_create();
	sendto(sock, buf, buf_l, 0,(struct sockaddr*)&sa, sizeof(sa));
	close(sock);
}

/* -------------------------------------------------------------------------- */

