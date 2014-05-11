/* -------------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "udp.h"

/* -------------------------------------------------------------------------- */

#define MAX_NODES 1000
#define MAX_EDGES 100

typedef struct node_t node_t;
struct node_t
{
	char addr[20];
	int port;

	node_t *edges[MAX_EDGES];
	int edges_l;
};

node_t nodes[MAX_NODES];
int nodes_l;

/* -------------------------------------------------------------------------- */

static void recv_cb(char *addr, int port, char *buf, int buf_l)
{
	printf("(%s:%d) -> %.*s\n", addr, port, buf_l, buf);

	/* buf -> init <id> <port> */
	if(buf[0] == 'i' && buf[1] == 'n' && buf[2] == 'i' && buf[3] == 't')
	{
		int id, port;
		sscanf(buf+4, "%d", &port);

		id = nodes_l++;
		strcpy(nodes[id].addr, addr);
		nodes[id].port = port;

		buf_l = sprintf(buf, "ok %d", id);
		ws_send(addr,port, buf, buf_l);

		printf("added node %d (%s:%d)\n", id, addr, port);
		return;
	}

	/* buf -> msg <id> <channel> <data> */
	if(buf[0] == 'm' && buf[1] == 's' && buf[2] == 'g') //msg
	{
		int id, i;

		sscanf(buf+3, " %d", &id);
		printf("Received msg from %d\n", id);

		for(i = 0; i < nodes[id].edges_l; i++)
		{
			printf("Redirecting to %s:%d\n", nodes[id].edges[i]->addr, nodes[id].edges[i]->port);
			ws_send(nodes[id].edges[i]->addr,nodes[id].edges[i]->port, buf, buf_l);
		}
	}
}

/* -------------------------------------------------------------------------- */

bool loop()
{
	char c;
	int f, t;

	if(scanf(" %c %d %d", &c, &f, &t) == EOF)
		return false;

	if(nodes[f].port == 0 || nodes[t].port == 0)
	{
		printf("Nodes not connected... (%d,%d)\n",nodes[f].port,nodes[t].port);
		return true;
	}

	if(c == 'a') // add edge
	{
		int i;
		for(i = 0; i < nodes[f].edges_l; i++)
			if(nodes[f].edges[i] == &nodes[t])
				return true;

		nodes[f].edges[nodes[f].edges_l++] = &nodes[t];
		printf("Added edge from %d to %d\n", f, t);
		return true;
	}

	if(c == 'd') // del edge
	{
		int i;
		for(i = 0; i < nodes[f].edges_l; i++)
		{
			if(nodes[f].edges[i] == &nodes[t])
			{
				nodes[f].edges[i] = nodes[f].edges[--nodes[f].edges_l];
				printf("Deleted edge from %d to %d\n", f, t);
				return true;
			}
		}
	}

	return true;
}

/* -------------------------------------------------------------------------- */

int main(void)
{
	ws_listen(recv_cb, 9999);
	while(loop());

	return 0;
}

/* -------------------------------------------------------------------------- */
