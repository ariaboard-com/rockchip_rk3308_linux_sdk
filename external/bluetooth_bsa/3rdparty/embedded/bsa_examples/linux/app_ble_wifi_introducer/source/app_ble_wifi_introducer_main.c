/*****************************************************************************
**
**  Name:           app_ble_wifi_introducer_main.c
**
**  Description:    Bluetooth BLE WiFi Introducer helper main application
**
**  Copyright (c) 2018, Cypress Corp., All Rights Reserved.
**  Cypress Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/
#include <stdlib.h>
#include <unistd.h>

#include "app_ble.h"
#include "bsa_avk_api.h"
#include "app_xml_param.h"
#include "app_utils.h"
#include "app_mgt.h"
#include "app_dm.h"
#include "app_manager.h"
#include "app_ble_wifi_introducer.h"

/*
 * Defines
 */
extern tAPP_MGR_CB app_mgr_cb;
int ble_wifi_config = 0;

/* BLE WIFI menu items */
enum
{
    APP_BLE_WIFI_INTRODUCER_MENU_SET_WIFI_JOIN_RETURN_VALUE_TO_TRUE = 1,
    APP_BLE_WIFI_INTRODUCER_MENU_SET_WIFI_JOIN_RETURN_VALUE_TO_FALSE,
    APP_BLE_WIFI_INTRODUCER_MENU_SEND_NOTIFICATION,
    APP_BLE_WIFI_INTRODUCER_MENU_DISPLAY,

    APP_BLE_WIFI_INTRODUCER_MENU_QUIT = 99
};

/*
 * Local functions
 */
static void app_ble_wifi_introducer_menu(void);
static void app_ble_wifi_introducer_display_menu (void);

/*******************************************************************************
 **
 ** Function         app_ble_wifi_introducer_display_menu
 **
 ** Description      This is the WIFI menu
 **
 ** Returns          void
 **
 *******************************************************************************/
static void app_ble_wifi_introducer_display_menu (void)
{
    APP_INFO0("\t**** APP BLE WIFI INTRODUCER menu ***");
    APP_INFO1("\t%d => Set WIFI Join return value to TRUE",
        APP_BLE_WIFI_INTRODUCER_MENU_SET_WIFI_JOIN_RETURN_VALUE_TO_TRUE);
    APP_INFO1("\t%d => Set WIFI Join return value to FALSE",
        APP_BLE_WIFI_INTRODUCER_MENU_SET_WIFI_JOIN_RETURN_VALUE_TO_FALSE);
    APP_INFO1("\t%d => Send Notification to Client",
        APP_BLE_WIFI_INTRODUCER_MENU_SEND_NOTIFICATION);
    APP_INFO1("\t%d => Display WiFi Introducer Sensor Information",
        APP_BLE_WIFI_INTRODUCER_MENU_DISPLAY);

    APP_INFO1("\t%d => Exit", APP_BLE_WIFI_INTRODUCER_MENU_QUIT);
}

/*******************************************************************************
 **
 ** Function         app_ble_wifi_introducer_menu
 **
 ** Description      WiFi introducer helper sub menu
 **
 ** Returns          void
 **
 *******************************************************************************/
void app_ble_wifi_introducer_menu(void)
{
#if 1
    while(ble_wifi_config) {
        sleep(1);
    }
#else
    int choice;
    do
    {
        app_ble_wifi_introducer_display_menu();

        choice = app_get_choice("Sub Menu");

        switch(choice)
        {
        case APP_BLE_WIFI_INTRODUCER_MENU_SET_WIFI_JOIN_RETURN_VALUE_TO_TRUE:
            app_ble_wifi_introducer_set_wifi_join_return_value_to_true();
            break;

        case APP_BLE_WIFI_INTRODUCER_MENU_SET_WIFI_JOIN_RETURN_VALUE_TO_FALSE:
            app_ble_wifi_introducer_set_wifi_join_return_value_to_false();
            break;

        case APP_BLE_WIFI_INTRODUCER_MENU_SEND_NOTIFICATION:
            app_ble_wifi_introducer_send_message();
            break;

        case APP_BLE_WIFI_INTRODUCER_MENU_DISPLAY:
            app_ble_wifi_introducer_display();
            break;

        case APP_BLE_WIFI_INTRODUCER_MENU_QUIT:
            APP_INFO0("Quit");
            break;

        default:
            APP_ERROR1("Unknown choice:%d", choice);
            break;
        }
    } while (choice != APP_BLE_WIFI_INTRODUCER_MENU_QUIT); /* While user don't exit sub-menu */
#endif
}

/*******************************************************************************
 **
 ** Function         app_ble_wifi_mgt_callback
 **
 ** Description      This callback function is called in case of server
 **                  disconnection (e.g. server crashes)
 **
 ** Parameters
 **
 ** Returns          BOOLEAN
 **
 *******************************************************************************/
BOOLEAN app_ble_wifi_mgt_callback(tBSA_MGT_EVT event, tBSA_MGT_MSG *p_data)
{
    switch(event)
    {
    case BSA_MGT_STATUS_EVT:
        APP_DEBUG0("BSA_MGT_STATUS_EVT");
        if (p_data->status.enable)
        {
            APP_DEBUG0("Bluetooth restarted => re-initialize the application");
            app_ble_start();
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

/*******************************************************************************
 **
 ** Function        main
 **
 ** Description     This is the main function
 **
 ** Parameters      Program's arguments
 **
 ** Returns         status
 **
 *******************************************************************************/
int main(int argc, char **argv)
{
    int status, mode;

    APP_INFO0("WiFi Introducer Sensor Start");

    ble_wifi_config = 1;

    app_ble_wifi_introducer_create_wifi_join_thread();

    /* Initialize BLE application */
    status = app_ble_init();
    if (status < 0)
    {
        APP_ERROR0("Couldn't Initialize BLE app, exiting");
        exit(-1);
    }

    /* Open connection to BSA Server */
    app_mgt_init();
    if (app_mgt_open(NULL, app_ble_wifi_mgt_callback) < 0) {
        APP_ERROR0("Unable to connect to server");
        return -1;
    }

    /* Init XML state machine */
    app_xml_init();

    if (app_mgr_config(NULL)) {
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

    /* Start BLE application */
    status = app_ble_start();
    if (status < 0) {
        APP_ERROR0("Couldn't Start BLE app, exiting");
        return -1;
    }

    app_ble_wifi_introducer_init();

    app_ble_wifi_introducer_gatt_server_init();

    /* The main BLE loop */
    app_ble_wifi_introducer_menu();

    /* Exit BLE mode */
    app_ble_exit();

    /* Close BSA Connection before exiting (to release resources) */
    app_mgt_close();

    APP_DEBUG0("exit app ble wifi introducer");
    exit(0);
}
