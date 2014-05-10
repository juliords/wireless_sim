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

#define LOCALHOST "127.0.0.1"

/* -------------------------------------------------------------------------- */

void* thread_listen(void *p)
{
	struct sockaddr_in sa; 
	socklen_t sa_len;
	char buf[1024];
	ssize_t recsize;

	int sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (-1 == sock)
	{
		printf("Error Creating Socket");
		exit(EXIT_FAILURE);
	}

	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = htonl(INADDR_ANY);
	sa.sin_port = htons(0);
	sa_len = sizeof(sa);

	if (-1 == bind(sock,(struct sockaddr *)&sa, sizeof(sa)))
	{
		perror("error bind failed");
		exit(EXIT_FAILURE);
	}

	getsockname(sock, (struct sockaddr *)&sa, &sa_len);
	printf("port: %d\n", ntohs(sa.sin_port));

	for (;;) 
	{
		recsize = recvfrom(sock, (void *)buf, sizeof(buf), 0, (struct sockaddr *)&sa, &sa_len);
		if (recsize < 0)
			continue;

		printf("%s:%d %.*s\n", inet_ntoa(sa.sin_addr), ntohs(sa.sin_port), (int)recsize, buf);
	}

	return NULL;
}

/* -------------------------------------------------------------------------- */

bool send_msg(struct sockaddr_in sa, char *buf, int blen)
{
	int sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	return sendto(sock, buf, blen, 0,(struct sockaddr*)&sa, sizeof(sa)) > 0;
}

bool tick()
{
	struct sockaddr_in sa;
	char addr[20] = LOCALHOST; int port;
	char buf[1024]; int blen;

	for (;;) 
	{
		if(scanf(" %s %d %1024[^\n]%n", addr, &port, buf, &blen) == EOF)
			return false;

		memset(&sa, 0, sizeof sa);
		sa.sin_family = AF_INET;
		sa.sin_addr.s_addr = inet_addr(addr);
		sa.sin_port = htons(port);

		send_msg(sa, buf, blen);
	}

	return true;
}

/* -------------------------------------------------------------------------- */

int main(void)
{
	pthread_t tid;

	pthread_create(&tid, NULL, thread_listen, NULL);
	pthread_detach(tid);

	while(tick());

	return 0;
}

/* -------------------------------------------------------------------------- */

