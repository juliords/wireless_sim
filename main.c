/* -------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdbool.h>
#include "udp.h"

/* -------------------------------------------------------------------------- */

static void dummy_recv_cb(char *addr, int port, int buf_l, char *buf)
{
	printf("%s:%d %.*s\n", addr, port, buf_l, buf);
}

/* -------------------------------------------------------------------------- */

bool loop()
{
	char addr[20]; int port;
	char buf[1024]; int buf_l;

	if(scanf(" %s %d %1024[^\n]%n", addr, &port, buf, &buf_l) == EOF)
		return false;

	ws_send(addr, port, buf, buf_l);

	return true;
}

/* -------------------------------------------------------------------------- */

int main(void)
{
	printf("## Wi-Fi (ethernet for now) Sim by JulioRdS ##\n");
	printf("Usage: <dest-ip> <dest-port> <message>\n");

	printf("my port -> %d\n", ws_listen(dummy_recv_cb, 0));
	while(loop());

	return 0;
}

/* -------------------------------------------------------------------------- */

