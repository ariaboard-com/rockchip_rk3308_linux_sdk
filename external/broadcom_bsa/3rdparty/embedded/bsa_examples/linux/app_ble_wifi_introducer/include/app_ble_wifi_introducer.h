/*****************************************************************************
**
**  Name:           app_ble_wifi_introducer.h
**
**  Description:    Bluetooth BLE WiFi introducer include file
**
**  Copyright (c) 2018, Cypress Corp., All Rights Reserved.
**  Cypress Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/
#ifndef APP_BLE_WIFI_INTRODUCER_H
#define APP_BLE_WIFI_INTRODUCER_H

#include "bsa_api.h"
#include "app_ble.h"

#define APP_BLE_WIFI_INTRODUCER_TIMEOUT 60 /* 6 seconds */
#define APP_BLE_WIFI_INTRODUCER_GATT_ATTRIBUTE_SIZE (22)
typedef void (tAPP_BLE_WIFI_CBACK)(tBSA_BLE_EVT event, tBSA_BLE_MSG *p_data);
/* This is the default AP the device will connect to (as a client)*/
#define CLIENT_AP_SSID       "YOUR_AP_SSID"
#define CLIENT_AP_PASSPHRASE "YOUR_AP_PASSPHRASE"

#ifndef APP_BLE_WIFI_INTRODUCER_ATTRIBUTE_MAX
#define APP_BLE_WIFI_INTRODUCER_ATTRIBUTE_MAX BSA_BLE_ATTRIBUTE_MAX
#endif

typedef struct
{
    tBT_UUID       attr_UUID;
    UINT16         service_id;
    UINT16         attr_id;
    UINT8          attr_type;
    tBSA_BLE_CHAR_PROP prop;
    tBSA_BLE_PERM       perm;
    BOOLEAN        is_pri;
    BOOLEAN        wait_flag;
    UINT16        val_len;
    UINT16        max_val_len;
    void            *p_val;
    tBSA_BLE_CBACK      *p_cback;
} tAPP_BLE_WIFI_INTRODUCER_ATTRIBUTE;

typedef struct 
{
    tBSA_BLE_IF         server_if;
    UINT16                conn_id;
    tAPP_BLE_WIFI_INTRODUCER_ATTRIBUTE  attr[APP_BLE_WIFI_INTRODUCER_ATTRIBUTE_MAX];
} tAPP_BLE_WIFI_INTRODUCER_CB;

/*******************************************************************************
 **
 ** Function           app_ble_wifi_introducer_create_wifi_join_thread
 **
 ** Description       Create Wifi Join Thread
 **
 ** Returns            nothing
 **
 *******************************************************************************/
int app_ble_wifi_introducer_create_wifi_join_thread(void);

/*******************************************************************************
 **
 ** Function         app_ble_wifi_introducer_init
 **
 ** Description     APP BLE wifi introducer control block init
 **
 ** Parameters     None
 **
 ** Returns          None
 **
 *******************************************************************************/
void app_ble_wifi_introducer_init(void);

/*******************************************************************************
 **
 ** Function         app_ble_wifi_introducer_gatt_server_init
 **
 ** Description     APP BLE wifi introducer GATT Server init
 **
 ** Parameters     None
 **
 ** Returns          None
 **
 *******************************************************************************/
void app_ble_wifi_introducer_gatt_server_init(void);

/*******************************************************************************
 **
 ** Function         app_ble_wifi_introducer_set_wifi_join_return_value_to_true
 **
 ** Description     as title
 **
 ** Parameters     None
 **
 ** Returns          None
 **
 *******************************************************************************/
void app_ble_wifi_introducer_set_wifi_join_return_value_to_true(void);

/*******************************************************************************
 **
 ** Function         app_ble_wifi_introducer_set_wifi_join_return_value_to_false
 **
 ** Description     as title
 **
 ** Parameters     None
 **
 ** Returns          None
 **
 *******************************************************************************/
void app_ble_wifi_introducer_set_wifi_join_return_value_to_false(void);

/*******************************************************************************
 **
 ** Function         app_ble_wifi_introducer_send_message
 **
 ** Description     Check if client has registered for notification/indication
 **                       and send message if appropriate
 **
 ** Parameters     None
 **
 ** Returns          None
 **
 *******************************************************************************/
void app_ble_wifi_introducer_send_message(void);

/*******************************************************************************
 **
 ** Function         app_ble_wifi_introducer_display
 **
 ** Description     Display the Variables
 **
 ** Parameters     None
 **
 ** Returns          None
 **
 *******************************************************************************/
void app_ble_wifi_introducer_display(void);
#endif
