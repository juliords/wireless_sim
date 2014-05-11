/* -------------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "udp.h"

/* -------------------------------------------------------------------------- */

char server_addr[20];
int server_port;
int my_port;
int my_id;

static void dummy_recv_cb(char *addr, int port, char *buf, int buf_l)
{
	printf("(%s:%d) -> %.*s\n", addr, port, buf_l, buf);

	/* buf -> init <id> <port> */
	if(buf[0] == 'o' && buf[1] == 'k')
	{
		int id;
		sscanf(buf+2, "%d", &id);

		my_id = id;

		printf("My id is %d\n", id);
		return;
	}

	/* buf -> msg <id> <channel> <data> */
	if(buf[0] == 'm' && buf[1] == 's' && buf[2] == 'g') //msg
	{
		// msg received
	}

}

/* -------------------------------------------------------------------------- */

bool loop()
{
	char buf[1024]; int buf_l, in_l;

	buf_l = sprintf(buf, "msg %d ", my_id);
	if(scanf(" %1024[^\n]%n", buf+buf_l, &in_l) == EOF)
		return false;

	ws_send(server_addr, server_port, buf, buf_l+in_l);

	return true;
}

/* -------------------------------------------------------------------------- */

void send_init()
{
	char buf[BUF_MAX];
	int buf_l;

	buf_l = sprintf(buf, "init %d", my_port);
	printf("Sending %.*s to %s:%d\n", buf_l, buf, server_addr, server_port);
	ws_send(server_addr, server_port, buf, buf_l);
}

int main(int argc, char **argv)
{
	if(argc != 3)
	{
		printf("Usage ./<bin> <server-ip> <server-port>\n");
		return 1;
	}

	strcpy(server_addr, argv[1]);
	server_port = atoi(argv[2]);

	my_port = ws_listen(dummy_recv_cb, 0);
	send_init();

	while(loop());

	return 0;
}

/* -------------------------------------------------------------------------- */

