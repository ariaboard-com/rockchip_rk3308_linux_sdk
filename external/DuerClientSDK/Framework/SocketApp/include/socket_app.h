#ifndef SOCKET_APP_H
#define SOCKET_APP_H

#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <sys/stat.h>

namespace duerOSDcsApp {
namespace framework {
/* Menu items, must be same as app_avk */
enum BtControlCmd {
    /* avk recieve */
    APP_AVK_MENU_DISCOVERY = 1,
    APP_AVK_MENU_REGISTER,
    APP_AVK_MENU_DEREGISTER,
    APP_AVK_MENU_OPEN,
    APP_AVK_MENU_CLOSE,
    APP_AVK_MENU_PLAY_START,
    APP_AVK_MENU_PLAY_STOP,
    APP_AVK_MENU_PLAY_PAUSE,
    APP_AVK_MENU_VOLUME_UP,
    APP_AVK_MENU_VOLUME_DOWN,
    APP_AVK_MENU_PLAY_NEXT_TRACK,
    APP_AVK_MENU_PLAY_PREVIOUS_TRACK,
    APP_AVK_MENU_RC_OPEN,
    APP_AVK_MENU_RC_CLOSE,
    APP_AVK_MENU_RC_CMD,
    APP_AVK_MENU_GET_ELEMENT_ATTR,
    APP_AVK_MENU_GET_CAPABILITIES,
    APP_AVK_MENU_REGISTER_NOTIFICATION,
    APP_AVK_MENU_REGISTER_NOTIFICATION_RESPONSE,
    APP_AVK_MENU_SEND_DELAY_RPT,
    APP_AVK_MENU_SEND_ABORT_REQ,
    APP_AVK_MENU_GET_PLAY_STATUS,
    APP_AVK_MENU_GET_ELEMENT_ATTRIBUTES,
    APP_AVK_MENU_SET_BROWSED_PLAYER,
    APP_AVK_MENU_SET_ADDRESSED_PLAYER,
    APP_AVK_MENU_GET_FOLDER_ITEMS,
    APP_AVK_MENU_CHANGE_PATH,
    APP_AVK_MENU_GET_ITEM_ATTRIBUTES,
    APP_AVK_MENU_PLAY_ITEM,
    APP_AVK_MENU_ADD_TO_NOW_PLAYING,
    APP_AVK_MENU_LIST_PLAYER_APP_SET_ATTR,
    APP_AVK_MENU_LIST_PLAYER_APP_SET_VALUE,
    APP_AVK_MENU_SET_ABSOLUTE_VOLUME,
    APP_AVK_MENU_SELECT_STREAMING_DEVICE,
#if (defined(AVRC_COVER_ART_INCLUDED) && AVRC_COVER_ART_INCLUDED == TRUE)
    APP_AVK_MENU_GET_COVER_ART_PROPERTY,
    APP_AVK_MENU_GET_COVER_ART_THUMBNAIL,
    APP_AVK_MENU_GET_COVER_ART_IMAGE,
    APP_AVK_MENU_COVERT_ART_ABORT,
#endif
    APP_AVK_MENU_QUIT = 99,

    /* avk send */
    APP_AVK_BT_CONNECT,
    APP_AVK_BT_DISCONNECT,
    APP_AVK_BT_PLAY,
    APP_AVK_BT_STOP,
    APP_AVK_BT_WAIT_PAIR,
    APP_AVK_BT_PAIR_SUCCESS,
    APP_AVK_BT_PAIR_FAILED_OTHER,
};

typedef struct{
    int server_sockfd;
    int client_sockfd;
    int server_len;
    int client_len;
    struct sockaddr_un server_address;
    struct sockaddr_un client_address;
    char sock_path[512];
} tSOCKET_APP;

/********************SERVER API***************************/
int setup_socket_server(tSOCKET_APP *app_socket);
int accpet_client(tSOCKET_APP *app_socket);
void teardown_socket_server(tSOCKET_APP *app_socket);

/********************CLIENT API***************************/
int setup_socket_client(char *socket_path);
void teardown_socket_client(int sockfd);
/********************COMMON API***************************/
int socket_send(int sockfd, char *msg, int len);
int socket_recieve(int sockfd, char *msg, int len);

} // namespace framework
}// namespace duerOSDcsApp

#endif
