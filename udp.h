#ifndef NETWORK_H
#define NETWORK_H

#define LOCALHOST "127.0.0.1"
#define BUF_MAX 1024

typedef void (*ws_recv_cb)(char *addr, int port, char *buf, int buf_l);

int ws_listen(ws_recv_cb cb, int port);

void ws_send(char *addr, int port, char *buf, int buf_l);

#endif
