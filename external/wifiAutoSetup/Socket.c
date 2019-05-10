#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <pthread.h>


#include "Socket.h"

#define BUFFER_SIZE 1024

int sock = -1;
int connected = -1;

void* socketclient(void* args)
{
    char recvbuf[BUFFER_SIZE];
    int result;
    while (connected == 0) {
        memset(recvbuf, 0, sizeof(recvbuf));
        result = recv(sock, recvbuf, sizeof(recvbuf), 0);
        if (result > 0)
        {
            close(sock);
        }
    }
}

int socket_connect(char *ip, int port)
{
    sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);  ///服务器端口
    servaddr.sin_addr.s_addr = inet_addr(ip);  ///服务器ip

    if (connect(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        connected = -1;
        return -1;
    }
    connected = 0;
    //pthread_t th;
    //pthread_create(&th, NULL, socketclient,NULL);
    return 0;
}

int socket_send(char *msg)
{
    if (connected == 0) {
        send(sock, msg, strlen(msg), 0);
        /*char recvbuf[BUFFER_SIZE];
        memset(recvbuf, 0, sizeof(recvbuf));
        recv(sock, recvbuf, sizeof(recvbuf), 0);*/
    }
}

void socket_close() {
    shutdown(sock, SHUT_RDWR); 
    close(sock);
    connected = -1;
}
/*
int main()
{

    if (0 == socket_connect("127.0.0.1", 1221)) {
        socket_send("This is a Test!");
        socket_close();
    }

    return 0;
}*/

