#ifndef SOCKET_H
#define SOCKET_H

int socket_connect(char *ip, int port);
int socket_send(char *msg);
void socket_close();

#endif
