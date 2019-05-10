/*****************************************************************************
 **
 **  Name:           app_avk_main.c
 **
 **  Description:    Bluetooth Manager application
 **
 **  Copyright (c) 2015, Broadcom Corp., All Rights Reserved.
 **  Broadcom Bluetooth Core. Proprietary and confidential.
 **
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "bsa_api.h"

#include "gki.h"
#include "uipc.h"

#include "app_avk.h"
#include "bsa_avk_api.h"
#include "app_xml_param.h"
#include "app_mgt.h"
#include "app_disc.h"
#include "app_utils.h"
#include "app_dm.h"
#include "app_manager.h"
#include "app_socket.h"

#ifdef DUEROS
extern tAPP_MGR_CB app_mgr_cb;
static int dueros_socket_fd = -1;
char sock_path[]="/data/bsa/config/socket_dueros";
#else
tAPP_SOCKET app_socket;
char sock_path[]="/data/bsa/config/socket_avk";
#endif

int app_avk_wait_close = 0;

/*******************************************************************************
 **
 ** Function         app_avk_display_main_menu
 **
 ** Description      This is the main menu
 **
 ** Parameters
 **
 ** Returns          void
 **
 *******************************************************************************/
void app_avk_display_main_menu(void)
{
    APP_INFO0("\nBluetooth AVK Main menu:");

    APP_INFO1("\t%d  => Start Discovery", APP_AVK_MENU_DISCOVERY);
    APP_INFO1("\t%d  => AVK Register", APP_AVK_MENU_REGISTER);
    APP_INFO1("\t%d  => AVK DeRegister", APP_AVK_MENU_DEREGISTER);
    APP_INFO1("\t%d  => AVK Open", APP_AVK_MENU_OPEN);
    APP_INFO1("\t%d  => AVK Close", APP_AVK_MENU_CLOSE);
    APP_INFO1("\t%d  => AVK Start steam play", APP_AVK_MENU_PLAY_START);
    APP_INFO1("\t%d  => AVK Stop stream play", APP_AVK_MENU_PLAY_STOP);
    APP_INFO1("\t%d  => AVK Pause stream play", APP_AVK_MENU_PLAY_PAUSE);
    APP_INFO1("\t%d  => AVK Play next track", APP_AVK_MENU_PLAY_NEXT_TRACK);
    APP_INFO1("\t%d  => AVK Play previous track", APP_AVK_MENU_PLAY_PREVIOUS_TRACK);
    APP_INFO1("\t%d  => AVRC Command", APP_AVK_MENU_RC_CMD);
    APP_INFO1("\t%d  => AVK Send Get Element Attributes Command", APP_AVK_MENU_GET_ELEMENT_ATTR);
    APP_INFO1("\t%d  => AVK Get capabilities", APP_AVK_MENU_GET_CAPABILITIES);
    APP_INFO1("\t%d  => AVK Register Notification", APP_AVK_MENU_REGISTER_NOTIFICATION);
    APP_INFO1("\t%d  => AVK Send Delay Report", APP_AVK_MENU_SEND_DELAY_RPT);
    APP_INFO1("\t%d  => Quit", APP_AVK_MENU_QUIT);
}


/*******************************************************************************
 **
 ** Function         app_avk_mgt_callback
 **
 ** Description      This callback function is called in case of server
 **                  disconnection (e.g. server crashes)
 **
 ** Parameters
 **
 ** Returns          TRUE if the event was completely handle, FALSE otherwise
 **
 *******************************************************************************/
static BOOLEAN app_avk_mgt_callback(tBSA_MGT_EVT event, tBSA_MGT_MSG *p_data)
{
    switch(event)
    {
    case BSA_MGT_STATUS_EVT:
        APP_DEBUG0("BSA_MGT_STATUS_EVT");
        if (p_data->status.enable)
        {
            APP_DEBUG0("Bluetooth restarted => re-initialize the application");
            app_avk_init(NULL);
        }
        break;

    case BSA_MGT_DISCONNECT_EVT:
        APP_DEBUG1("BSA_MGT_DISCONNECT_EVT reason:%d", p_data->disconnect.reason);
        /* Connection with the Server lost => Just exit the application */
        exit(-1);
        break;

    default:
        break;
    }
    return FALSE;
}

static void app_avk_cmd_send_callback(int msg, int status) {
    int cmd = -1;

    APP_DEBUG1("app_avk_cmd_send_callback, msg: %d, status: %d\n", msg, status);
    switch (msg) {
        case BSA_SEC_LINK_UP_EVT:
            cmd = APP_AVK_BT_CONNECT;
            break;

        case BSA_SEC_LINK_DOWN_EVT:
            if(app_avk_wait_close) {
                cmd = APP_AVK_BT_DISCONNECT;
                app_avk_wait_close = 0;
            }
            break;

        case BSA_SEC_SP_CFM_REQ_EVT:
            cmd = APP_AVK_BT_WAIT_PAIR;
            break;

        case BSA_SEC_AUTH_CMPL_EVT:
            if(status)
                cmd = APP_AVK_BT_PAIR_SUCCESS;
            else
                cmd = APP_AVK_BT_PAIR_FAILED_OTHER;
            break;
    }

    if(cmd >= 0) {
        if (app_avk_socket_send(cmd) < 0)
            APP_DEBUG1("avk socket send failed, cmd: %d\n", cmd);
    }
}

int app_avk_socket_send(int cmd) {
    int ret = -1;

#ifdef DUEROS
    char msg[10];
    memset(msg, 0, 10);
    sprintf(msg, "%d", cmd);

    APP_DEBUG1("msg: %s, len: %d\n", msg, strlen(msg));
    ret = socket_send(dueros_socket_fd, msg, strlen(msg));
#endif

    return ret;
}

/*******************************************************************************
 **
 ** Function         main
 **
 ** Description      This is the main function to connect to AVK. It is assumed that an other process handle security.
 **
 ** Parameters
 **
 ** Returns          void
 **
 *******************************************************************************/
#ifdef DUEROS
int main(int argc, char **argv)
{
    int choice, bytes, mode;
    char msg[64];

    /* Open connection to BSA Server */
    app_mgt_init();
    if (app_mgt_open(NULL, app_avk_mgt_callback) < 0) {
        APP_ERROR0("Unable to connect to server");
        return -1;
    }

    /* Init XML state machine */
    app_xml_init();

    if (app_mgr_config(app_avk_cmd_send_callback)) {
        APP_ERROR0("Couldn't configure successfully, exiting");
        return -1;
    }

    /* Display FW versions */
    app_mgr_read_version();

    /* Get the current Stack mode */
    mode = app_dm_get_dual_stack_mode();
    if (mode < 0) {
        APP_ERROR0("app_dm_get_dual_stack_mode failed");
        return -1;
    } else {
        /* Save the current DualStack mode */
        app_mgr_cb.dual_stack_mode = mode;
        APP_INFO1("Current DualStack mode:%s", app_mgr_get_dual_stack_mode_desc());
    }

    app_avk_init(NULL);

    dueros_socket_fd = setup_socket_client(sock_path);
    if (dueros_socket_fd < 0) {
        APP_ERROR0("Fail to connect server socket\n");
        return 0;
    }

    APP_DEBUG0("Service connected\n");

    do {
        memset(msg, 0, sizeof(msg));
        bytes = socket_recieve(dueros_socket_fd, msg, sizeof(msg));
        if (bytes == 0 ) {
            APP_DEBUG0("server leaved, break\n");
            break;
        }

        choice = atoi(msg);
        APP_DEBUG1("msg = %s, choice :%d\n", msg, choice);

        if (choice == APP_AVK_MENU_REGISTER)
            app_avk_register();
        else
            app_avk_rc_send_cmd(choice);
    } while(choice != APP_AVK_MENU_QUIT);

    /* Terminate the profile */
    app_avk_end();
    /* Close BSA before exiting (to release resources) */
    app_mgt_close();
    teardown_socket_client(dueros_socket_fd);

    APP_DEBUG0("exit app avk");
    return 0;
}

#else
int main(int argc, char **argv)
{
    int choice, avrcp_evt, bytes, i;
    int connection_index;
    UINT32 first_param, second_param, third_param;
    tAVRC_UID uid;
    UINT16 delay;
    tAPP_AVK_CONNECTION *connection = NULL;
    char msg[64];
    int use_socket = 0;

    for (i = 1; i < argc; i++)
    {
        char *arg = argv[i];
        if (*arg != '-')
        {
            APP_ERROR1("Unsupported parameter #%d : %s", i+1, argv[i]);
            exit(-1);
        }
        /* Bypass the first '-' char */
        arg++;
        /* check if the second char is also a '-'char */
        if (*arg == '-') arg++;
        switch (*arg)
        {
            case 's':
                use_socket = 1;
                break;
            default:
                break;
        }
    }

    /* Open connection to BSA Server */
    app_mgt_init();
    if (app_mgt_open(NULL, app_avk_mgt_callback) < 0)
    {
        APP_ERROR0("Unable to connect to server");
        return -1;
    }

    /* Init XML state machine */
    app_xml_init();

    app_avk_init(NULL);

    if (use_socket == 1) {
        strcpy(app_socket.sock_path, sock_path);
        if ((setup_socket_server(&app_socket)) < 0)
            return 0;
        if (accpet_client(&app_socket) < 0)
            return 0;
        printf("client connted\n");
    }
    do
    {
        app_avk_display_main_menu();

        if (use_socket == 0) {
            choice = app_get_choice("Select action");
        } else {
            memset(msg,0,sizeof(msg));
            bytes = socket_recieve(app_socket.client_sockfd, msg, sizeof(msg));
            if (bytes == 0 ) {
                printf("client leaved, waiting for reconnect\n");
                if (accpet_client(&app_socket) < 0)
                    return 0;
                continue;
            }

            choice = atoi(msg);
            printf("msg = %s, choice :%d\n",msg, choice);
        }

        switch (choice)
        {

        case APP_AVK_MENU_DISCOVERY:
            /* Example to perform Device discovery (in blocking mode) */
            app_disc_start_regular(NULL);
            break;

        case APP_AVK_MENU_REGISTER:
            /* Example to register AVK connection end point*/
            app_avk_register();
            break;
        case APP_AVK_MENU_DEREGISTER:
            /* Example to de-register an AVK end point */
            app_avk_deregister();
            break;
        case APP_AVK_MENU_OPEN:
            /* Example to Open AVK connection (connect device)*/
            app_avk_open();
            break;

        case APP_AVK_MENU_CLOSE:
            /* Example to Close AVK connection (disconnect device)*/
            printf("Choose connection index\n");
            app_avk_display_connections();
            connection_index = app_get_choice("\n");
            connection = app_avk_find_connection_by_index(connection_index);
            if(connection)
            app_avk_close(connection->bda_connected);
            else
                printf("Unknown choice:%d\n", connection_index);
            break;

        case APP_AVK_MENU_PLAY_START:
            /* Example to start stream play */
            printf("Choose connection index\n");
            app_avk_display_connections();
            connection_index = app_get_choice("\n");
            connection = app_avk_find_connection_by_index(connection_index);
            if(connection)
            app_avk_play_start(connection->rc_handle);
            else
                printf("Unknown choice:%d\n", connection_index);
            break;

        case APP_AVK_MENU_PLAY_STOP:
            /* Example to stop stream play */
            printf("Choose connection index\n");
            app_avk_display_connections();
            connection_index = app_get_choice("\n");
            connection = app_avk_find_connection_by_index(connection_index);
            if(connection)
            app_avk_play_stop(connection->rc_handle);
            else
                printf("Unknown choice:%d\n", connection_index);
            break;  

        case APP_AVK_MENU_PLAY_PAUSE:
            /* Example to pause stream play */
            printf("Choose connection index\n");
            app_avk_display_connections();
            connection_index = app_get_choice("\n");
            connection = app_avk_find_connection_by_index(connection_index);
            if(connection)
            app_avk_play_pause(connection->rc_handle);
            else
                printf("Unknown choice:%d\n", connection_index);
            break;

        case APP_AVK_MENU_VOLUME_UP:
            /* Example to volume up */
            printf("tiantian, volume up\n");
            printf("Choose connection index\n");
            app_avk_display_connections();
            connection_index = app_get_choice("\n");
            connection = app_avk_find_connection_by_index(connection_index);
            if(connection)
                app_avk_volume_up(connection->rc_handle);
            else
                printf("Unknown choice:%d\n", connection_index);
            break;

        case APP_AVK_MENU_VOLUME_DOWN:
            /* Example to volume down */
            printf("tiantian, volume down\n");
            printf("Choose connection index\n");
            app_avk_display_connections();
            connection_index = app_get_choice("\n");
            connection = app_avk_find_connection_by_index(connection_index);
            if(connection)
                app_avk_volume_down(connection->rc_handle);
            else
                printf("Unknown choice:%d\n", connection_index);
            break;

        case APP_AVK_MENU_PLAY_NEXT_TRACK:
            /* Example to play next track */
            printf("Choose connection index\n");
            app_avk_display_connections();
            connection_index = app_get_choice("\n");
            connection = app_avk_find_connection_by_index(connection_index);
            if(connection)
             app_avk_play_next_track(connection->rc_handle);
            else
                printf("Unknown choice:%d\n", connection_index);
            break;

        case APP_AVK_MENU_PLAY_PREVIOUS_TRACK:
            /* Example to play previous track */
            printf("Choose connection index\n");
            app_avk_display_connections();
            connection_index = app_get_choice("\n");
            connection = app_avk_find_connection_by_index(connection_index);
            if(connection)
            app_avk_play_previous_track(connection->rc_handle);
            else
                printf("Unknown choice:%d\n", connection_index);
            break;  

        case APP_AVK_MENU_RC_CMD:
            /* Example to send AVRC cmd */
            printf("Choose connection index\n");
            app_avk_display_connections();
            connection_index = app_get_choice("\n");
            connection = app_avk_find_connection_by_index(connection_index);
            if(connection)
            app_avk_rc_cmd(connection->rc_handle);
            else
                printf("Unknown choice:%d\n", connection_index);
            break;          

        case APP_AVK_MENU_GET_ELEMENT_ATTR:
            /* Example to send Vendor Dependent command */
            printf("Choose connection index\n");
            app_avk_display_connections();
            connection_index = app_get_choice("\n");
            connection = app_avk_find_connection_by_index(connection_index);
            if(connection)
            app_avk_send_get_element_attributes_cmd(connection->rc_handle);
            else
                printf("Unknown choice:%d\n", connection_index);
            break;

        case APP_AVK_MENU_GET_CAPABILITIES:
            /* Example to send Vendor Dependent command */
            printf("Choose connection index\n");
            app_avk_display_connections();
            connection_index = app_get_choice("\n");
            connection = app_avk_find_connection_by_index(connection_index);
            if(connection)
            app_avk_send_get_capabilities(connection->rc_handle);
            else
                printf("Unknown choice:%d\n", connection_index);
            break;

        case APP_AVK_MENU_REGISTER_NOTIFICATION:
            /* Example to send Vendor Dependent command */
            printf("Choose connection index\n");
            app_avk_display_connections();
            connection_index = app_get_choice("\n");
            connection = app_avk_find_connection_by_index(connection_index);
            if(connection)
            {
            printf("Choose Event ID\n");
            printf("    1=play_status 2=track_change 5=play_pos 8=app_set\n");
            printf("    13=volume_changed\n");
            avrcp_evt = app_get_choice("\n");
            app_avk_send_register_notification(avrcp_evt, connection->rc_handle);
            }
            else
                printf("Unknown choice:%d\n", connection_index);
            break;

        case APP_AVK_MENU_SEND_DELAY_RPT:
            APP_INFO0("delay value(0.1ms unit)");
            delay = app_get_choice("\n");
            app_avk_send_delay_report(delay);
            break;

        case APP_AVK_MENU_QUIT:
            printf("main: Bye Bye\n");
            break;

        default:
            printf("main: Unknown choice:%d\n", choice);
            break;
        }
    } while (choice != APP_AVK_MENU_QUIT); /* While user don't exit application */

    /* Terminate the profile */
    app_avk_end();


    /* Close BSA before exiting (to release resources) */
    app_mgt_close();

    if (use_socket == 1)
            teardown_socket_server(&app_socket);

    return 0;
}
#endif
