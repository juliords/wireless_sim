#ifndef NETWORK_H
#define NETWORK_H

typedef void (*ws_recv_cb)(char *addr, int port, int buf_l, char *buf);

int ws_listen(ws_recv_cb cb, int port);

void ws_send(char *addr, int port, char *buf, int buf_l);

#endif
