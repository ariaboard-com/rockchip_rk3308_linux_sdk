/*
 * duer_link_tcp_connection_observer.h
 *
 *  Created on: 2018年6月5日
 *      Author: wangning
 */

#ifndef DUER_LINK_TCP_CONNECTION_OBSERVER_H
#define DUER_LINK_TCP_CONNECTION_OBSERVER_H

#include <string>

class DuerLinkTcpConnectionObserver {
public:
    virtual int dlp_create_tcp_socket() = 0;
    virtual int dlp_connect_tcp_socket(
            int socket_fd, const struct sockaddr* addr_name, int name_len) = 0;
    virtual int dlp_send_tcp_data(int socket_fd, const void* send_buffer, int send_buffer_len) = 0;
    virtual int dlp_recv_tcp_data(int socket_fd, void* recv_buffer, int recv_buffer_len) = 0;
    virtual void dlp_close_tcp_socket(int& socket_fd) = 0;
    virtual void dlp_shutdown_tcp_socket(int& socket_fd, int how) = 0;
};
#endif /* DUER_LINK_TCP_CONNECTION_OBSERVER_H */
