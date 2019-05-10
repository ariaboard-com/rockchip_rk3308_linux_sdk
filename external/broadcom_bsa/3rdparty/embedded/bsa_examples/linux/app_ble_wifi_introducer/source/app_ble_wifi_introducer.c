/*****************************************************************************
**
**  Name:           app_ble_wifi_introducer.c
**
**  Description:    Bluetooth BLE WiFi Introducer main application
**
**  Copyright (c) 2014, Cypress Corp., All Rights Reserved.
**  Cypress Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include "bsa_api.h"
#include "app_ble.h"
#include "app_thread.h"
#include "app_mutex.h"
#include "app_utils.h"
#include "app_dm.h"
#include "app_socket.h"
#include "app_ble_wifi_introducer.h"

/*
* Defines
*/
#define APP_BLE_WIFI_INTRODUCER_UUID        0xFEAA
//#define BSA_BLE_GAP_ADV_SLOW_INT           2048         /* Tgap(adv_slow_interval) = 1.28 s= 512 * 0.625 ms */
#define BSA_BLE_GAP_ADV_SLOW_INT           2056
#define BSA_BLE_CONNECT_EVT BTA_BLE_CONNECT_EVT
#define BTM_BLE_ADVERT_TYPE_NAME_COMPLETE 0x09

/*
 * Global Variables
 */

/*
 * Local Variables
 */
extern int ble_wifi_config;
static tAPP_THREAD  join_thread;
static tAPP_MUTEX join_mutex;
static BOOLEAN wifi_join_return_value = TRUE; // This is for simulate Wifi Join Function

#ifdef DUEROS
BD_NAME wifi_introducer_device_name;
#else
static UINT8 wifi_introducer_device_name[ ] = "WiFiInt"; // This is for Adv only
#endif

static UINT8 attr_index_notify;

/* UUID value of Apollo config network info */
static UINT8 wifi_introducer_service_uuid[LEN_UUID_128] =
        {0x23, 0x20, 0x56, 0x7c, 0x05, 0xcf, 0x6e, 0xb4, 0xc3, 0x41, 0x77, 0x28, 0x51, 0x82, 0x7e, 0x1b};
static UINT8 wifi_introducer_characteristic_nw_security_uuid[LEN_UUID_128] =
        {0xa1, 0x93, 0xcd, 0xa5, 0x84, 0x0a, 0xaf, 0xbb, 0x4a, 0x4c, 0xbb, 0xed, 0xa4, 0xab, 0xc2, 0xca};
static UINT8 wifi_introducer_characteristic_nw_ssid_uuid[LEN_UUID_128] =
        {0x56, 0xb3, 0xf6, 0xce, 0xa6, 0x19, 0x08, 0x95, 0xad, 0x48, 0xaa, 0xee, 0x7c, 0xef, 0xa0, 0xac};
static UINT8 wifi_introducer_characteristic_nw_passphrase_uuid[LEN_UUID_128] =
        {0xce, 0xa6, 0x15, 0xb4, 0x33, 0xd8, 0x76, 0xa8, 0x8b, 0x4c, 0xe4, 0x93, 0x33, 0xde, 0xb7, 0x40};

/* UUID value to  the Hello Sensor Characteristic, Value Notification */
static UINT8 wifi_introducer_characteristic_notify_uuid[LEN_UUID_128] =
        {0x26, 0xf6, 0x69, 0x91, 0x68, 0xee, 0xc2, 0xbe, 0x44, 0x4d, 0xb9, 0x5c, 0x3f, 0x2d, 0xc3, 0x8a};

static UINT16 wifi_introducer_characteristic_notify_characteristic_client_configuration = 0;
static UINT8 wifi_introducer_char_nw_security_value = 0 /*WICED_SECURITY_OPEN*/;
static UINT8 wifi_introducer_char_nw_ssid_value[APP_BLE_WIFI_INTRODUCER_GATT_ATTRIBUTE_SIZE] =
                        CLIENT_AP_SSID;
static UINT8 wifi_introducer_char_nw_passphrase_value[APP_BLE_WIFI_INTRODUCER_GATT_ATTRIBUTE_SIZE]  =
                        CLIENT_AP_PASSPHRASE;
static UINT8 wifi_introducer_char_notify_value = 0; // 0 means not configured , 1 means configured
static UINT8 wifi_introducer_char_battery_level_value = 0;

static tAPP_BLE_WIFI_INTRODUCER_CB app_ble_wifi_introducer_cb;
static BOOLEAN wifi_introducer_ssid_name         = FALSE;
static BOOLEAN wifi_introducer_ssid_password   = FALSE;

#ifdef DUEROS
/* DuerOS wifi introducer info */
#define DUEROS_WIFI_CONFIG_SERVICE_UUID         0x1111
#define DUEROS_WIFI_CONFIG_CHARACTERISTIC_UUID  0x2222
#define DUEROS_WIFI_CONFIG_DESCRIPTOR_UUID      0x2902

/* Must be the same as the maximum length of data sent by ble_send_data_cb,
 * otherwise the data will be transmitted to the dueros app will be wrong,
 * the reason is unknown.
 */
#define DUEROS_SOCKET_RECV_LEN 20
#define DUEROS_CHARACTERISTIC_VALUE       "DUEROS_VALUE"
static UINT8 dueros_characteristic_value[APP_BLE_WIFI_INTRODUCER_GATT_ATTRIBUTE_SIZE] =
                        DUEROS_CHARACTERISTIC_VALUE;

static pthread_t dueros_tid = 0;
static int dueros_socket_done = 0;
static int dueros_socket_fd = -1;
static char dueros_socket_path[] = "/data/bsa/config/socket_dueros";

static void dueros_set_device_name(void);
static int dueros_socket_send(char *msg, int len);
static void *dueros_socket_recieve(void *arg);
static int dueros_socket_thread_create(void);
static void dueros_socket_thread_delete(void);
static int dueros_wifi_introducer_create_gatt_database(void);
static void dueros_wifi_introducer_ssid_password_callback(tBSA_BLE_EVT event,
                  tBSA_BLE_MSG *p_data);
static void dueros_wifi_introducer_ccc_callback(tBSA_BLE_EVT event, tBSA_BLE_MSG *p_data);
#endif

/*
 * Local functions
 */
static int app_ble_wifi_introducer_register(void);
static void app_ble_wifi_introducer_set_advertisement_data(void);
static int app_ble_wifi_introducer_create_gatt_database(void);
static int app_ble_wifi_introducer_create_service(tBT_UUID *service_uuid,
               UINT16 num_handle);
static int app_ble_wifi_introducer_start_service(UINT16 service_id);
static int app_ble_wifi_introducer_add_char(tAPP_BLE_WIFI_INTRODUCER_ATTRIBUTE *attr);
static void app_ble_wifi_introducer_profile_cback(tBSA_BLE_EVT event,
                    tBSA_BLE_MSG *p_data);
static int app_ble_wifi_introducer_find_free_attr(void);
static int app_ble_wifi_introducer_find_attr_index_by_attr_id(UINT16 attr_id);
static void app_ble_wifi_introducer_ssid_name_callback(tBSA_BLE_EVT event,
                    tBSA_BLE_MSG *p_data);
static void app_ble_wifi_introducer_ssid_password_callback(tBSA_BLE_EVT event,
                    tBSA_BLE_MSG *p_data);
static void is_ssid_configured(void);
static void app_ble_wifi_introducer_ccc_callback(tBSA_BLE_EVT event,
                    tBSA_BLE_MSG *p_data);
static void app_ble_wifi_introducer_nw_security_callback(tBSA_BLE_EVT event,
                    tBSA_BLE_MSG *p_data);
static void app_ble_wifi_introducer_battery_level_callback(tBSA_BLE_EVT event,
                    tBSA_BLE_MSG *p_data);
static void app_ble_wifi_introducer_wifi_join_thread(void);
static int app_ble_wifi_introducer_send_notification(void);

/*
 * BLE common functions
 */
 
/*******************************************************************************
 **
 ** Function        app_ble_wifi_introducer_register
 **
 ** Description     Register app
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
static int app_ble_wifi_introducer_register(void)
{
    tBSA_STATUS status;
    tBSA_BLE_SE_REGISTER ble_register_param;

    status = BSA_BleSeAppRegisterInit(&ble_register_param);
    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("BSA_BleSeAppRegisterInit failed status = %d", status);
        return -1;
    }

    ble_register_param.uuid.len = LEN_UUID_16;
    ble_register_param.uuid.uu.uuid16 = APP_BLE_WIFI_INTRODUCER_UUID;
    ble_register_param.p_cback = app_ble_wifi_introducer_profile_cback;

    status = BSA_BleSeAppRegister(&ble_register_param);
    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("BSA_BleSeAppRegister failed status = %d", status);
        return -1;
    }
    app_ble_wifi_introducer_cb.server_if = ble_register_param.server_if;
    APP_INFO1("server_if:%d", app_ble_wifi_introducer_cb.server_if);
    return 0;
}

/************************************* ******************************************
 **
 ** Function        app_ble_wifi_introducer_set_advertisement_data
 **
 ** Description     Setup advertisement data with 16 byte UUID and device name
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
static void app_ble_wifi_introducer_set_advertisement_data(void)
{
    tBSA_DM_BLE_AD_MASK data_mask;
    tBSA_DM_SET_CONFIG bt_config;
    tBSA_STATUS bsa_status;
    UINT8 len = 0;

    /* Set Bluetooth configuration */
    BSA_DmSetConfigInit(&bt_config);

    /* Obviously */
    bt_config.enable = TRUE;

    /* Configure the Advertisement Data parameters */
    bt_config.config_mask = BSA_DM_CONFIG_BLE_ADV_CONFIG_MASK;

    /* Use services flag to show above services if required on the peer device */
#ifdef DUEROS
    data_mask = BSA_DM_BLE_AD_BIT_FLAGS | BSA_DM_BLE_AD_BIT_PROPRIETARY |
                         BSA_DM_BLE_AD_BIT_SERVICE;
#else
    data_mask = BSA_DM_BLE_AD_BIT_FLAGS | BSA_DM_BLE_AD_BIT_PROPRIETARY |
                         BSA_DM_BLE_AD_BIT_SERVICE_128;
#endif

    bt_config.adv_config.flag = BSA_DM_BLE_GEN_DISC_FLAG | BSA_DM_BLE_BREDR_NOT_SPT;
    bt_config.adv_config.adv_data_mask = data_mask;
    bt_config.adv_config.is_scan_rsp = FALSE;

#ifdef DUEROS
    bt_config.adv_config.num_service = 1;
    bt_config.adv_config.uuid_val[0] = DUEROS_WIFI_CONFIG_SERVICE_UUID;
    len += bt_config.adv_config.num_service * sizeof(UINT16);
#else
    memcpy(bt_config.adv_config.services_128b.uuid128,
                   wifi_introducer_service_uuid, LEN_UUID_128);
    bt_config.adv_config.services_128b.list_cmpl = TRUE;
    len += LEN_UUID_128;
#endif

    bt_config.adv_config.proprietary.num_elem = 1;
    bt_config.adv_config.proprietary.elem[0].adv_type = BTM_BLE_ADVERT_TYPE_NAME_COMPLETE;
    len += 2;

    bt_config.adv_config.proprietary.elem[0].len = strlen((char *)wifi_introducer_device_name);
    strcpy((char *)bt_config.adv_config.proprietary.elem[0].val, (char *)wifi_introducer_device_name);
    len += bt_config.adv_config.proprietary.elem[0].len;
    bt_config.adv_config.len = len;

    bsa_status = BSA_DmSetConfig(&bt_config);
    if (bsa_status != BSA_SUCCESS)
    {
        APP_ERROR1("BSA_DmSetConfig failed status:%d ", bsa_status);
        return;
    }
    APP_INFO0("app_ble_wifi_introducer_set_advertisement_data");
}

/*******************************************************************************
 **
 ** Function        app_ble_wifi_introducer_create_gatt_database
 **
 ** Description     This is the GATT database for the WiFi Introducer Sensor application.
 **                       It defines services, characteristics and descriptors supported by the sensor.
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
static int app_ble_wifi_introducer_create_gatt_database(void)
{
    int srvc_attr_index;
    tBT_UUID service_uuid;
    tAPP_BLE_WIFI_INTRODUCER_ATTRIBUTE attr;

    APP_INFO0("app_ble_wifi_introducer_create_gatt_database");

    /* create a BLE service */
    service_uuid.len = LEN_UUID_128;
    memcpy(service_uuid.uu.uuid128, wifi_introducer_service_uuid, MAX_UUID_SIZE);
    srvc_attr_index = app_ble_wifi_introducer_create_service(&service_uuid, 20);
    if (srvc_attr_index < 0)
    {
        APP_ERROR0("Service Create Fail");
        return -1;
    }

    /* Declare characteristic used to notify/indicate change */
    memset(&attr, 0, sizeof(tAPP_BLE_WIFI_INTRODUCER_ATTRIBUTE));
    attr.attr_UUID.len = LEN_UUID_128;
    memcpy(&attr.attr_UUID.uu.uuid128, wifi_introducer_characteristic_notify_uuid,
                     MAX_UUID_SIZE);
    attr.service_id = app_ble_wifi_introducer_cb.attr[srvc_attr_index].service_id;
    attr.perm = BSA_GATT_PERM_READ;
    attr.prop = BSA_GATT_CHAR_PROP_BIT_READ | BSA_GATT_CHAR_PROP_BIT_NOTIFY |
                    BSA_GATT_CHAR_PROP_BIT_INDICATE;
    attr.attr_type = BSA_GATTC_ATTR_TYPE_CHAR;
    attr.val_len = attr.max_val_len = sizeof(wifi_introducer_char_notify_value);
    attr.p_val = &wifi_introducer_char_notify_value;
    attr_index_notify = app_ble_wifi_introducer_add_char(&attr);

     /* Declare client characteristic configuration descriptor
       * Value of the descriptor can be modified by the client
       * Value modified shall be retained during connection and across connection
       * for bonded devices.  Setting value to 1 tells this application to send notification
       * when value of the characteristic changes.  Value 2 is to allow indications. */
    memset(&attr, 0, sizeof(tAPP_BLE_WIFI_INTRODUCER_ATTRIBUTE));
    attr.attr_UUID.len = LEN_UUID_16;
    attr.attr_UUID.uu.uuid16 = GATT_UUID_CHAR_CLIENT_CONFIG;
    attr.service_id = app_ble_wifi_introducer_cb.attr[srvc_attr_index].service_id;
    attr.perm = BSA_GATT_PERM_READ | BSA_GATT_PERM_WRITE;
    attr.attr_type = BSA_GATTC_ATTR_TYPE_CHAR_DESCR;
    attr.val_len = attr.max_val_len = sizeof(UINT16);
    attr.p_val = &wifi_introducer_characteristic_notify_characteristic_client_configuration;
    attr.p_cback = app_ble_wifi_introducer_ccc_callback;
    app_ble_wifi_introducer_add_char(&attr);

    /* Declare characteristic for Security  */
    memset(&attr, 0, sizeof(tAPP_BLE_WIFI_INTRODUCER_ATTRIBUTE));
    attr.attr_UUID.len = LEN_UUID_128;
    memcpy(&attr.attr_UUID.uu.uuid128, wifi_introducer_characteristic_nw_security_uuid, MAX_UUID_SIZE);
    attr.service_id = app_ble_wifi_introducer_cb.attr[srvc_attr_index].service_id;
    attr.perm = BSA_GATT_PERM_READ | BSA_GATT_PERM_READ_ENCRYPTED |
                       BSA_GATT_PERM_WRITE | BSA_GATT_PERM_WRITE_ENCRYPTED;
    attr.prop = BSA_GATT_CHAR_PROP_BIT_READ | BSA_GATT_CHAR_PROP_BIT_WRITE;
    attr.attr_type = BSA_GATTC_ATTR_TYPE_CHAR;
    attr.val_len = attr.max_val_len = sizeof(UINT8);
    attr.p_val = &wifi_introducer_char_nw_security_value;
    attr.p_cback = app_ble_wifi_introducer_nw_security_callback;
    app_ble_wifi_introducer_add_char(&attr);

    /* Declare characteristic for SSID  */
    memset(&attr, 0, sizeof(tAPP_BLE_WIFI_INTRODUCER_ATTRIBUTE));
    attr.attr_UUID.len = LEN_UUID_128;
    memcpy(&attr.attr_UUID.uu.uuid128, wifi_introducer_characteristic_nw_ssid_uuid,
                     MAX_UUID_SIZE);
    attr.service_id = app_ble_wifi_introducer_cb.attr[srvc_attr_index].service_id;
    attr.perm = BSA_GATT_PERM_READ | BSA_GATT_PERM_READ_ENCRYPTED |
                       BSA_GATT_PERM_WRITE | BSA_GATT_PERM_WRITE_ENCRYPTED;
    attr.prop = BSA_GATT_CHAR_PROP_BIT_READ | BSA_GATT_CHAR_PROP_BIT_WRITE;
    attr.attr_type = BSA_GATTC_ATTR_TYPE_CHAR;
    attr.val_len = strlen(CLIENT_AP_SSID);
    attr.max_val_len = APP_BLE_WIFI_INTRODUCER_GATT_ATTRIBUTE_SIZE - 1; // reserve one byte for end of string
    attr.p_val = wifi_introducer_char_nw_ssid_value;
    attr.p_cback = app_ble_wifi_introducer_ssid_name_callback;
    app_ble_wifi_introducer_add_char(&attr);

    /* Declare characteristic for Passphrase  */
    memset(&attr, 0, sizeof(tAPP_BLE_WIFI_INTRODUCER_ATTRIBUTE));
    attr.attr_UUID.len = LEN_UUID_128;
    memcpy(&attr.attr_UUID.uu.uuid128, wifi_introducer_characteristic_nw_passphrase_uuid,
                     MAX_UUID_SIZE);
    attr.service_id = app_ble_wifi_introducer_cb.attr[srvc_attr_index].service_id;
    attr.perm = BSA_GATT_PERM_READ | BSA_GATT_PERM_READ_ENCRYPTED |
                       BSA_GATT_PERM_WRITE | BSA_GATT_PERM_WRITE_ENCRYPTED;
    attr.prop = BSA_GATT_CHAR_PROP_BIT_READ | BSA_GATT_CHAR_PROP_BIT_WRITE;
    attr.attr_type = BSA_GATTC_ATTR_TYPE_CHAR;
    attr.val_len = strlen(CLIENT_AP_PASSPHRASE);
    attr.max_val_len = APP_BLE_WIFI_INTRODUCER_GATT_ATTRIBUTE_SIZE - 1; // reserve one byte for end of string
    attr.p_val = wifi_introducer_char_nw_passphrase_value;
    attr.p_cback = app_ble_wifi_introducer_ssid_password_callback;
    app_ble_wifi_introducer_add_char(&attr);

    /* Declare Device Info service */
    service_uuid.len = LEN_UUID_16;
    service_uuid.uu.uuid16 = UUID_SERVCLASS_DEVICE_INFO;
    srvc_attr_index = app_ble_wifi_introducer_create_service(&service_uuid, 10);
    if (srvc_attr_index < 0)
    {
        APP_ERROR0("Device Info Service Create Fail");
        return -1;
    }

    /* Declare characteristic Manufacturer Name */
    memset(&attr, 0, sizeof(tAPP_BLE_WIFI_INTRODUCER_ATTRIBUTE));
    attr.attr_UUID.len = LEN_UUID_16;
    attr.attr_UUID.uu.uuid16 = GATT_UUID_MANU_NAME;
    attr.service_id = app_ble_wifi_introducer_cb.attr[srvc_attr_index].service_id;
    attr.perm = BSA_GATT_PERM_READ;
    attr.prop = BSA_GATT_CHAR_PROP_BIT_READ;
    attr.attr_type = BSA_GATTC_ATTR_TYPE_CHAR;
    app_ble_wifi_introducer_add_char(&attr);

    /* Declare characteristic Model Number */
    memset(&attr, 0, sizeof(tAPP_BLE_WIFI_INTRODUCER_ATTRIBUTE));
    attr.attr_UUID.len = LEN_UUID_16;
    attr.attr_UUID.uu.uuid16 = GATT_UUID_MODEL_NUMBER_STR;
    attr.service_id = app_ble_wifi_introducer_cb.attr[srvc_attr_index].service_id;
    attr.perm = BSA_GATT_PERM_READ;
    attr.prop = BSA_GATT_CHAR_PROP_BIT_READ;
    attr.attr_type = BSA_GATTC_ATTR_TYPE_CHAR;
    app_ble_wifi_introducer_add_char(&attr);

    /* Declare characteristic System ID */
    memset(&attr, 0, sizeof(tAPP_BLE_WIFI_INTRODUCER_ATTRIBUTE));
    attr.attr_UUID.len = LEN_UUID_16;
    attr.attr_UUID.uu.uuid16 = GATT_UUID_SYSTEM_ID;
    attr.service_id = app_ble_wifi_introducer_cb.attr[srvc_attr_index].service_id;
    attr.perm = BSA_GATT_PERM_READ;
    attr.prop = BSA_GATT_CHAR_PROP_BIT_READ;
    attr.attr_type = BSA_GATTC_ATTR_TYPE_CHAR;
    app_ble_wifi_introducer_add_char(&attr);

    /* Declare Battery service */
    service_uuid.len = LEN_UUID_16;
    service_uuid.uu.uuid16 = UUID_SERVCLASS_BATTERY;
    srvc_attr_index = app_ble_wifi_introducer_create_service(&service_uuid, 10);
    if (srvc_attr_index < 0)
    {
        APP_ERROR0("Battery Service Create Fail");
        return -1;
    }

    /* Declare characteristic Battery Level */
    memset(&attr, 0, sizeof(tAPP_BLE_WIFI_INTRODUCER_ATTRIBUTE));
    attr.attr_UUID.len = LEN_UUID_16;
    attr.attr_UUID.uu.uuid16 = GATT_UUID_BATTERY_LEVEL;
    attr.service_id = app_ble_wifi_introducer_cb.attr[srvc_attr_index].service_id;
    attr.perm = BSA_GATT_PERM_READ;
    attr.prop = BSA_GATT_CHAR_PROP_BIT_READ;
    attr.attr_type = BSA_GATTC_ATTR_TYPE_CHAR;
    attr.val_len = attr.max_val_len = sizeof(wifi_introducer_char_battery_level_value);
    attr.p_val = &wifi_introducer_char_battery_level_value;
    attr.p_cback = app_ble_wifi_introducer_battery_level_callback;
    app_ble_wifi_introducer_add_char(&attr);
    return 0;
}

/*******************************************************************************
 **
 ** Function        app_ble_wifi_introducer_create_service
 **
 ** Description     create service
 **
 ** Parameters     service UUID
 **                       number of handle for reserved
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
static int app_ble_wifi_introducer_create_service(tBT_UUID *service_uuid,
               UINT16 num_handle)
{
    tBSA_STATUS status;
    tBSA_BLE_SE_CREATE ble_create_param;
    int attr_index = -1;
    UINT32 timeout = APP_BLE_WIFI_INTRODUCER_TIMEOUT;

    attr_index = app_ble_wifi_introducer_find_free_attr();
    if (attr_index < 0)
    {
        APP_ERROR1("Wrong attr number! = %d", attr_index);
        return -1;
    }

    status = BSA_BleSeCreateServiceInit(&ble_create_param);
    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("BSA_BleSeCreateServiceInit failed status = %d", status);
        return -1;
    }

    memcpy(&ble_create_param.service_uuid, service_uuid, sizeof(tBT_UUID));
    ble_create_param.server_if = app_ble_wifi_introducer_cb.server_if;
    ble_create_param.num_handle = num_handle;
    ble_create_param.is_primary = TRUE;

    app_ble_wifi_introducer_cb.attr[attr_index].wait_flag = TRUE;

    if ((status = BSA_BleSeCreateService(&ble_create_param)) == BSA_SUCCESS)
    {
        while (app_ble_wifi_introducer_cb.attr[attr_index].wait_flag && timeout)
        {
            GKI_delay(100);
            timeout--;
        }
    }
    if ((status != BSA_SUCCESS) || (timeout == 0))
    {
        APP_ERROR1("BSA_BleSeCreateService failed status = %d", status);
        app_ble_wifi_introducer_cb.attr[attr_index].wait_flag = FALSE;
        return -1;
    }

    /* store information on control block */
    memcpy(&app_ble_wifi_introducer_cb.attr[attr_index].attr_UUID, service_uuid,
                    sizeof(tBT_UUID));
    app_ble_wifi_introducer_cb.attr[attr_index].is_pri = ble_create_param.is_primary;
    app_ble_wifi_introducer_cb.attr[attr_index].attr_type = BSA_GATTC_ATTR_TYPE_SRVC;
    return attr_index;
}

/*******************************************************************************
 **
 ** Function        app_ble_wifi_introducer_start_service
 **
 ** Description     Start Service
 **
 ** Parameters      service_id : attr id
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
static int app_ble_wifi_introducer_start_service(UINT16 service_id)
{
    tBSA_STATUS status;
    tBSA_BLE_SE_START ble_start_param;

    status = BSA_BleSeStartServiceInit(&ble_start_param);
    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("BSA_BleSeStartServiceInit failed status = %d", status);
        return -1;
    }

    ble_start_param.service_id = service_id;
    ble_start_param.sup_transport = BSA_BLE_GATT_TRANSPORT_LE;

    APP_INFO1("service_id:%d", ble_start_param.service_id);

    status = BSA_BleSeStartService(&ble_start_param);
    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("BSA_BleSeStartService failed status = %d", status);
        return -1;
    }
    return 0;
}

/*******************************************************************************
 **
 ** Function        app_ble_wifi_introducer_add_char
 **
 ** Description     Add character to service
 **
 ** Parameters      tAPP_BLE_WIFI_INTRODUCER_ATTRIBUTE
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
static int app_ble_wifi_introducer_add_char(tAPP_BLE_WIFI_INTRODUCER_ATTRIBUTE *attr)
{
    tBSA_STATUS status;
    tBSA_BLE_SE_ADDCHAR ble_addchar_param;
    int attr_index = -1;
    UINT32 timeout = APP_BLE_WIFI_INTRODUCER_TIMEOUT;

    attr_index = app_ble_wifi_introducer_find_free_attr();
    if (attr_index < 0)
    {
        APP_ERROR1("Wrong attr index! = %d", attr_index);
        return -1;
    }

    status = BSA_BleSeAddCharInit(&ble_addchar_param);
    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("BSA_BleSeAddCharInit failed status = %d", status);
        return -1;
    }

    /* characteristic */
    ble_addchar_param.service_id = attr->service_id;
    ble_addchar_param.is_descr = (attr->attr_type == BSA_GATTC_ATTR_TYPE_CHAR_DESCR);
    memcpy(&ble_addchar_param.char_uuid, &attr->attr_UUID, sizeof(tBT_UUID));
    ble_addchar_param.perm = attr->perm;
    ble_addchar_param.property = attr->prop;

    APP_INFO1("app_ble_wifi_introducer_add_char service_id:%d", ble_addchar_param.service_id);
    app_ble_wifi_introducer_cb.attr[attr_index].wait_flag = TRUE;

    if ((status = BSA_BleSeAddChar(&ble_addchar_param)) == BSA_SUCCESS)
    {
        while (app_ble_wifi_introducer_cb.attr[attr_index].wait_flag && timeout)
        {
            GKI_delay(100);
            timeout--;
        }
    }
    if ((status != BSA_SUCCESS) || (timeout == 0))
    {
        APP_ERROR1("BSA_BleSeAddChar failed status = %d", status);
        return -1;
    }
	
    /* store information on control block */
    memcpy(&app_ble_wifi_introducer_cb.attr[attr_index].attr_UUID, &attr->attr_UUID,
                    sizeof(tBT_UUID));
    app_ble_wifi_introducer_cb.attr[attr_index].attr_type = attr->attr_type;
    app_ble_wifi_introducer_cb.attr[attr_index].val_len = attr->val_len;
    app_ble_wifi_introducer_cb.attr[attr_index].max_val_len = attr->max_val_len;
    app_ble_wifi_introducer_cb.attr[attr_index].p_val = attr->p_val;
    app_ble_wifi_introducer_cb.attr[attr_index].p_cback = attr->p_cback;
    return attr_index;
}

/*******************************************************************************
**
** Function         app_ble_wifi_introducer_profile_cback
**
** Description      APP BLE wifi introducer Profile callback.
**
** Returns          void
**
*******************************************************************************/
static void app_ble_wifi_introducer_profile_cback(tBSA_BLE_EVT event,
                   tBSA_BLE_MSG *p_data)
{
    int attr_index;
    tBSA_BLE_SE_SENDRSP send_server_resp;
    tBSA_DM_BLE_ADV_PARAM adv_param;
    int attr_len_to_copy;

    APP_DEBUG1("event = %d ", event);

    switch (event)
    {
    case BSA_BLE_SE_DEREGISTER_EVT:
        APP_INFO1("BSA_BLE_SE_DEREGISTER_EVT server_if:%d status:%d",
            p_data->ser_deregister.server_if, p_data->ser_deregister.status);
        if (p_data->ser_deregister.server_if != app_ble_wifi_introducer_cb.server_if)
        {
            APP_ERROR0("wrong deregister interface!!");
            break;
        }

        app_ble_wifi_introducer_cb.server_if = BSA_BLE_INVALID_IF;
        for (attr_index = 0; attr_index < APP_BLE_WIFI_INTRODUCER_ATTRIBUTE_MAX; attr_index++)
        {
            memset(&app_ble_wifi_introducer_cb.attr[attr_index], 0,
                              sizeof(tAPP_BLE_WIFI_INTRODUCER_ATTRIBUTE));
        }
        break;

    case BSA_BLE_SE_CREATE_EVT:
        APP_INFO1("BSA_BLE_SE_CREATE_EVT server_if:%d status:%d service_id:%d",
            p_data->ser_create.server_if, p_data->ser_create.status, p_data->ser_create.service_id);

        /* search interface number */
        if (p_data->ser_create.server_if != app_ble_wifi_introducer_cb.server_if)
        {
            APP_ERROR0("interface wrong!!");
            break;
        }

        /* search attribute number */
        for (attr_index = 0; attr_index < APP_BLE_WIFI_INTRODUCER_ATTRIBUTE_MAX; attr_index++)
        {
            if (app_ble_wifi_introducer_cb.attr[attr_index].wait_flag == TRUE)
            {
                APP_INFO1("BSA_BLE_SE_CREATE_EVT attr_index:%d", attr_index);
                if (p_data->ser_create.status == BSA_SUCCESS)
                {
                    app_ble_wifi_introducer_cb.attr[attr_index].service_id = p_data->ser_create.service_id;
                    app_ble_wifi_introducer_cb.attr[attr_index].attr_id = p_data->ser_create.service_id;
                    app_ble_wifi_introducer_cb.attr[attr_index].wait_flag = FALSE;
                    break;
                }
                else  /* if CREATE fail */
                {
                    memset(&app_ble_wifi_introducer_cb.attr[attr_index], 0, sizeof(tAPP_BLE_WIFI_INTRODUCER_ATTRIBUTE));
                    break;
                }
            }
        }
        if (attr_index >= APP_BLE_WIFI_INTRODUCER_ATTRIBUTE_MAX)
        {
            APP_ERROR0("BSA_BLE_SE_CREATE_EVT no waiting!!");
            break;
        }
        break;

    case BSA_BLE_SE_ADDCHAR_EVT:
        APP_INFO1("BSA_BLE_SE_ADDCHAR_EVT status:%d", p_data->ser_addchar.status);
        APP_INFO1("attr_id:0x%x", p_data->ser_addchar.attr_id);

        /* search attribute number */
        for (attr_index = 0; attr_index < APP_BLE_WIFI_INTRODUCER_ATTRIBUTE_MAX; attr_index++)
        {
            if (app_ble_wifi_introducer_cb.attr[attr_index].wait_flag == TRUE)
            {
                APP_INFO1("BSA_BLE_SE_ADDCHAR_EVT attr_index:%d", attr_index);
                if (p_data->ser_addchar.status == BSA_SUCCESS)
                {
                    app_ble_wifi_introducer_cb.attr[attr_index].service_id = p_data->ser_addchar.service_id;
                    app_ble_wifi_introducer_cb.attr[attr_index].attr_id = p_data->ser_addchar.attr_id;
                    app_ble_wifi_introducer_cb.attr[attr_index].wait_flag = FALSE;
                    break;
                }
                else  /* if ADD fail */
                {
                    memset(&app_ble_wifi_introducer_cb.attr[attr_index], 0, sizeof(tAPP_BLE_WIFI_INTRODUCER_ATTRIBUTE));
                    break;
                }
            }
        }
        if (attr_index >= APP_BLE_WIFI_INTRODUCER_ATTRIBUTE_MAX)
        {
            APP_ERROR0("BSA_BLE_SE_ADDCHAR_EVT no waiting!!");
            break;
        }
        break;

    case BSA_BLE_SE_START_EVT:
        APP_INFO1("BSA_BLE_SE_START_EVT status:%d", p_data->ser_start.status);

#ifdef DUEROS
        dueros_socket_thread_create();
#endif
        break;

    case BSA_BLE_SE_STOP_EVT:
        APP_INFO1("BSA_BLE_SE_STOP_EVT status:%d", p_data->ser_stop.status);
        break;

    case BSA_BLE_SE_READ_EVT:
        APP_INFO1("BSA_BLE_SE_READ_EVT status:%d, handle:%d", p_data->ser_read.status, p_data->ser_read.handle);
        BSA_BleSeSendRspInit(&send_server_resp);
        send_server_resp.conn_id = p_data->ser_read.conn_id;
        send_server_resp.trans_id = p_data->ser_read.trans_id;
        send_server_resp.status = p_data->ser_read.status;
        send_server_resp.handle = p_data->ser_read.handle;
        send_server_resp.offset = p_data->ser_read.offset;
	    attr_index = app_ble_wifi_introducer_find_attr_index_by_attr_id(p_data->ser_read.handle);
	    APP_INFO1("BSA_BLE_SE_READ_EVT attr_index:%d", attr_index);
        if (attr_index < 0)
        {
            APP_ERROR0("Cannot find matched attr_id");
	     break;
        }
        attr_len_to_copy = app_ble_wifi_introducer_cb.attr[attr_index].val_len;
        if (p_data->ser_read.offset >= attr_len_to_copy)
            attr_len_to_copy = send_server_resp.len = 0;
        if (attr_len_to_copy != 0)
        {
            attr_len_to_copy -= p_data->ser_read.offset;
        }
        send_server_resp.len = attr_len_to_copy;
        send_server_resp.auth_req = GATT_AUTH_REQ_NONE;
        memcpy(send_server_resp.value, app_ble_wifi_introducer_cb.attr[attr_index].p_val + p_data->ser_read.offset,
                      send_server_resp.len);
        APP_INFO1("BSA_BLE_SE_READ_EVT: send_server_resp.conn_id:%d, send_server_resp.trans_id:%d", send_server_resp.conn_id, send_server_resp.trans_id, send_server_resp.status);
        APP_INFO1("BSA_BLE_SE_READ_EVT: send_server_resp.status:%d,send_server_resp.auth_req:%d", send_server_resp.status,send_server_resp.auth_req);
        APP_INFO1("BSA_BLE_SE_READ_EVT: send_server_resp.handle:%d, send_server_resp.offset:%d, send_server_resp.len:%d", send_server_resp.handle,send_server_resp.offset,send_server_resp.len );
        BSA_BleSeSendRsp(&send_server_resp);
	 if (app_ble_wifi_introducer_cb.attr[attr_index].p_cback)
           app_ble_wifi_introducer_cb.attr[attr_index].p_cback(event, p_data);
        break;

    case BSA_BLE_SE_WRITE_EVT:
        APP_INFO1("BSA_BLE_SE_WRITE_EVT status:%d", p_data->ser_write.status);
        APP_DUMP("Write value", p_data->ser_write.value, p_data->ser_write.len);
        APP_INFO1("BSA_BLE_SE_WRITE_EVT trans_id:%d, conn_id:%d, handle:%d",
            p_data->ser_write.trans_id, p_data->ser_write.conn_id, p_data->ser_write.handle);

	    attr_index = app_ble_wifi_introducer_find_attr_index_by_attr_id(p_data->ser_read.handle);
        APP_INFO1("BSA_BLE_SE_WRITE_EVT attr_index:%d", attr_index);
        if (attr_index < 0)
        {
            APP_ERROR0("Cannot find matched attr_id");
	        break;
        }
        if (p_data->ser_write.len >= app_ble_wifi_introducer_cb.attr[attr_index].max_val_len)
            p_data->ser_write.len = app_ble_wifi_introducer_cb.attr[attr_index].max_val_len;
        app_ble_wifi_introducer_cb.attr[attr_index].val_len = p_data->ser_write.len;
        memcpy(app_ble_wifi_introducer_cb.attr[attr_index].p_val, p_data->ser_write.value,
                       p_data->ser_write.len);
        if (p_data->ser_write.need_rsp)
        {
            BSA_BleSeSendRspInit(&send_server_resp);
            send_server_resp.conn_id = p_data->ser_write.conn_id;
            send_server_resp.trans_id = p_data->ser_write.trans_id;
            send_server_resp.status = p_data->ser_write.status;
            send_server_resp.handle = p_data->ser_write.handle;
            send_server_resp.len = 0;
            APP_INFO1("BSA_BLE_SE_WRITE_EVT: send_server_resp.conn_id:%d, send_server_resp.trans_id:%d", send_server_resp.conn_id, send_server_resp.trans_id, send_server_resp.status);
            APP_INFO1("BSA_BLE_SE_WRITE_EVT: send_server_resp.status:%d,send_server_resp.auth_req:%d", send_server_resp.status,send_server_resp.auth_req);
            APP_INFO1("BSA_BLE_SE_WRITE_EVT: send_server_resp.handle:%d, send_server_resp.offset:%d, send_server_resp.len:%d", send_server_resp.handle,send_server_resp.offset,send_server_resp.len );
            BSA_BleSeSendRsp(&send_server_resp);
        }
	 if (app_ble_wifi_introducer_cb.attr[attr_index].p_cback)
           app_ble_wifi_introducer_cb.attr[attr_index].p_cback(event, p_data);
        break;

    case BSA_BLE_SE_EXEC_WRITE_EVT:
        APP_INFO1("BSA_BLE_SE_EXEC_WRITE_EVT status:%d", p_data->ser_exec_write.status);
        APP_INFO1("BSA_BLE_SE_EXEC_WRITE_EVT trans_id:%d, conn_id:%d, flag:%d",
            p_data->ser_exec_write.trans_id, p_data->ser_exec_write.conn_id,
            p_data->ser_exec_write.flag);

#if 0
        BSA_BleSeSendRspInit(&send_server_resp);
        send_server_resp.conn_id = p_data->ser_exec_write.conn_id;
        send_server_resp.trans_id = p_data->ser_exec_write.trans_id;
        send_server_resp.status = p_data->ser_exec_write.status;
        send_server_resp.handle = p_data->ser_exec_write.handle;
        send_server_resp.len = 0;
        APP_INFO1("conn_id:%d, trans_id:%d",
            send_server_resp.conn_id, send_server_resp.trans_id);
        APP_INFO1("status:%d, auth_req:%d",
            send_server_resp.status,send_server_resp.auth_req);
        APP_INFO1("handle:%d, offset:%d, len:%d",
            send_server_resp.handle,send_server_resp.offset,send_server_resp.len );
        BSA_BleSeSendRsp(&send_server_resp);
#endif
        break;


    case BSA_BLE_SE_OPEN_EVT:
        APP_INFO1("BSA_BLE_SE_OPEN_EVT status:%d", p_data->ser_open.reason);
        if (p_data->ser_open.reason == BSA_SUCCESS)
        {
            APP_INFO1("app_ble_wifi_introducer_conn_up conn_id:0x%x", p_data->ser_open.conn_id);
            app_ble_wifi_introducer_cb.conn_id = p_data->ser_open.conn_id;

            APP_INFO1("app_ble_wifi_introducer_conn_up connected to [%02X:%02X:%02X:%02X:%02X:%02X]",
                          p_data->ser_open.remote_bda[0],
                          p_data->ser_open.remote_bda[1],
                          p_data->ser_open.remote_bda[2],
                          p_data->ser_open.remote_bda[3],
                          p_data->ser_open.remote_bda[4],
                          p_data->ser_open.remote_bda[5]);

            /* Stop advertising */
            app_dm_set_ble_visibility(FALSE, FALSE);
            APP_INFO0("Stopping Advertisements");
            wifi_introducer_characteristic_notify_characteristic_client_configuration = 0;
        }
        break;

    case BSA_BLE_SE_CONGEST_EVT:
        APP_INFO1("BSA_BLE_SE_CONGEST_EVT  :conn_id:0x%x, congested:%d",
            p_data->ser_congest.conn_id, p_data->ser_congest.congested);
        break;

    case BSA_BLE_SE_CLOSE_EVT:
        APP_INFO1("BSA_BLE_SE_CLOSE_EVT status:%d", p_data->ser_close.reason);
        APP_INFO1("conn_id:0x%x", p_data->ser_close.conn_id);
        APP_INFO1("app_ble_wifi_introducer_connection_down  conn_id:%d reason:%d", p_data->ser_close.conn_id, p_data->ser_close.reason);

#ifdef DUEROS
        dueros_socket_thread_delete();
#endif
        ble_wifi_config = 0;

        app_ble_wifi_introducer_cb.conn_id = BSA_BLE_INVALID_CONN;

        /* start low advertisements */
        /* Set ADV params */
        memset(&adv_param, 0, sizeof(tBSA_DM_BLE_ADV_PARAM));
        adv_param.adv_type = BSA_BLE_CONNECT_EVT;
        adv_param.adv_int_min = BSA_BLE_GAP_ADV_SLOW_INT;
        adv_param.adv_int_max = BSA_BLE_GAP_ADV_SLOW_INT;
        app_dm_set_ble_adv_param(&adv_param);
        /* Set visisble and connectable */
        app_dm_set_ble_visibility(TRUE, TRUE);
        break;

    case BSA_BLE_SE_CONF_EVT:  // It maybe same with BSA_BLE_SE_CONFIRM_EVT
        APP_INFO1("BSA_BLE_SE_CONF_EVT  :conn_id:0x%x, status:%d",
            p_data->ser_conf.conn_id, p_data->ser_conf.status);
        break;

    default:
        break;
    }
}

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
void app_ble_wifi_introducer_init(void)
{
    memset(&app_ble_wifi_introducer_cb, 0, sizeof(app_ble_wifi_introducer_cb));
    app_ble_wifi_introducer_cb.conn_id = BSA_BLE_INVALID_CONN;

#ifdef DUEROS
    dueros_set_device_name();
#endif

    app_init_mutex(&join_mutex);
}

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
void app_ble_wifi_introducer_gatt_server_init(void)
{
    int index;

    APP_INFO0("wifi_introducer_gatt_server_init");
    /* register BLE server app */
    /* Register with stack to receive GATT callback */
    app_ble_wifi_introducer_register();
    GKI_delay(1000);

#ifdef DUEROS
    dueros_wifi_introducer_create_gatt_database();
#else
    app_ble_wifi_introducer_create_gatt_database();
#endif

    /* start service */
    for (index = 0; index < APP_BLE_WIFI_INTRODUCER_ATTRIBUTE_MAX; index++)
    {
        if (app_ble_wifi_introducer_cb.attr[index].attr_type == BSA_GATTC_ATTR_TYPE_SRVC)
        {
            app_ble_wifi_introducer_start_service(app_ble_wifi_introducer_cb.attr[index].service_id);
        }
    }

    GKI_delay(1000);

    /* Set the advertising parameters */
    app_ble_wifi_introducer_set_advertisement_data();

    /* Set visisble and connectable */
    app_dm_set_visibility(FALSE, FALSE);
    app_dm_set_ble_visibility(TRUE, TRUE);

}

/*******************************************************************************
 **
 ** Function         app_ble_wifi_introducer_find_free_attr
 **
 ** Description      find free attr for APP BLE wifi introducer application
 **
 ** Parameters
 **
 ** Returns          positive number(include 0) if successful, error code otherwise
 **
 *******************************************************************************/
static int app_ble_wifi_introducer_find_free_attr(void)
{
    int index;

    for (index = 0; index < APP_BLE_WIFI_INTRODUCER_ATTRIBUTE_MAX; index++)
    {
        if (!app_ble_wifi_introducer_cb.attr[index].attr_UUID.uu.uuid16)
        {
            return index;
        }
    }
    return -1;
}

/*******************************************************************************
 **
 ** Function         app_ble_wifi_introducer_find_free_attr
 **
 ** Description      find free attr for APP BLE wifi introducer application
 **
 ** Parameters     attr_id
 **
 ** Returns          positive number(include 0) if successful, error code otherwise
 **
 *******************************************************************************/
static int app_ble_wifi_introducer_find_attr_index_by_attr_id(UINT16 attr_id)
{
    int index;

    for (index = 0; index < APP_BLE_WIFI_INTRODUCER_ATTRIBUTE_MAX; index++)
    {
        if (app_ble_wifi_introducer_cb.attr[index].attr_id == attr_id)
        {
            return index;
        }
    }
    return -1;
}

/*******************************************************************************
**
** Function         app_ble_wifi_introducer_ssid_name_callback
**
** Description      APP BLE wifi introducer SSID callback.
**
** Returns          void
**
*******************************************************************************/
static void app_ble_wifi_introducer_ssid_name_callback(tBSA_BLE_EVT event,
                  tBSA_BLE_MSG *p_data)
{
    int attr_index;

    APP_INFO0("app_ble_wifi_introducer_ssid_name_callback");
    if (event == BSA_BLE_SE_WRITE_EVT)
    {
	 attr_index = app_ble_wifi_introducer_find_attr_index_by_attr_id(p_data->ser_write.handle);
        if (attr_index < 0)
        {
            APP_ERROR0("Cannot find matched attr_id");
	     return;
        }
        if (app_ble_wifi_introducer_cb.attr[attr_index].val_len >= APP_BLE_WIFI_INTRODUCER_GATT_ATTRIBUTE_SIZE)
        {
             APP_ERROR0("Wrong wifi_introducer_char_nw_ssid_value length");
	     return;
        }
        wifi_introducer_char_nw_ssid_value[app_ble_wifi_introducer_cb.attr[attr_index].val_len] = 0;
        wifi_introducer_ssid_name = TRUE;
        APP_INFO1("write ssid value: %s", wifi_introducer_char_nw_ssid_value);
        is_ssid_configured();
    }
}

/*******************************************************************************
**
** Function         app_ble_wifi_introducer_ssid_password_callback
**
** Description      APP BLE wifi introducer Password callback.
**
** Returns          void
**
*******************************************************************************/
static void app_ble_wifi_introducer_ssid_password_callback(tBSA_BLE_EVT event,
                  tBSA_BLE_MSG *p_data)
{
    int attr_index;

    APP_INFO0("app_ble_wifi_introducer_ssid_password_callback");
    if (event == BSA_BLE_SE_WRITE_EVT)
    {
	 attr_index = app_ble_wifi_introducer_find_attr_index_by_attr_id(p_data->ser_write.handle);
        if (attr_index < 0)
        {
            APP_ERROR0("Cannot find matched attr_id");
	     return;
        }
        if (app_ble_wifi_introducer_cb.attr[attr_index].val_len >= APP_BLE_WIFI_INTRODUCER_GATT_ATTRIBUTE_SIZE)
        {
             APP_ERROR0("Wrong wifi_introducer_char_nw_passphrase_value length");
	     return;
        }
        wifi_introducer_char_nw_passphrase_value[app_ble_wifi_introducer_cb.attr[attr_index].val_len] = 0;
        wifi_introducer_ssid_password= TRUE;
        APP_INFO1("write passphrase value: %s", wifi_introducer_char_nw_passphrase_value);
        is_ssid_configured();
    }
}

/*******************************************************************************
**
** Function         start_wpa_supplicant
**
** Description      wpa supplicant
**
** Returns          BOOLEAN
**
*******************************************************************************/
static BOOLEAN start_wpa_supplicant(void)
{
    FILE *fp = NULL;
    if ((fp = fopen("/data/bsa/wpa_supplicant.conf", "w+")) == NULL)
    {
        APP_ERROR0("open bsa wpa_supplicant.conf failed");
        return FALSE;
    }

    fprintf(fp, "%s\n", "ctrl_interface=/var/run/wpa_supplicant");
    fprintf(fp, "%s\n", "ap_scan=1");
    fprintf(fp, "%s\n", "network={");
    fprintf(fp, "%s%s%s\n", "ssid=\"", wifi_introducer_char_nw_ssid_value, "\"");
    fprintf(fp, "%s%s%s\n", "psk=\"", wifi_introducer_char_nw_passphrase_value, "\"");
    fprintf(fp, "%s\n", "key_mgmt=WPA-PSK");
    fprintf(fp, "%s\n", "}");

    fclose(fp);

    if (-1 == system("killall wpa_supplicant;killall dhcpcd;"
                   "ifconfig wlan0 0.0.0.0")) {
        APP_ERROR0("killall wpa_supplicant dhcpcd failed");
        return FALSE;
    }

    if (-1 == system("wpa_supplicant -Dnl80211 -i wlan0 "
                   "-c /data/bsa/wpa_supplicant.conf &")) {
        APP_ERROR0("start wpa_supplicant failed");
        return FALSE;
    }

    if (-1 == system("sleep 1;dhcpcd wlan0 -t 0 &")) {
        APP_ERROR0("dhcpcd failed");
        return FALSE;
    }

    return TRUE;
}

/*******************************************************************************
**
** Function         is_ssid_configured
**
** Description      Check if configured
**
** Returns          void
**
*******************************************************************************/
static void is_ssid_configured(void)
{
    if (wifi_introducer_ssid_name && wifi_introducer_ssid_password)
    {
        APP_INFO0("SSID configured");

        if (!start_wpa_supplicant())
        {
            APP_ERROR0("start wpa_supplicant failed");
        }

        app_unlock_mutex(&join_mutex);
    }
}

/*******************************************************************************
**
** Function         app_ble_wifi_introducer_battery_level_callback
**
** Description      APP BLE wifi introducer Battery Level callback.
**                        Dummy battery value read increment
**
** Returns          void
**
*******************************************************************************/
static void app_ble_wifi_introducer_battery_level_callback(tBSA_BLE_EVT event,
                  tBSA_BLE_MSG *p_data)
{
    if ((event == BSA_BLE_SE_READ_EVT) && (wifi_introducer_char_battery_level_value++ > 99))
        wifi_introducer_char_battery_level_value = 0;
}

/*******************************************************************************
**
** Function         app_ble_wifi_introducer_ccc_callback
**
** Description      APP BLE wifi introducer CCC callback.
**
** Returns          void
**
*******************************************************************************/
static void app_ble_wifi_introducer_ccc_callback(tBSA_BLE_EVT event,
                  tBSA_BLE_MSG *p_data)
{
    if (event == BSA_BLE_SE_WRITE_EVT)
    {
        APP_INFO0("app_ble_wifi_introducer_ccc_callback");
        APP_INFO1("wifi_introducer_characteristic_notify_characteristic_client_configuration = %d" ,
           wifi_introducer_characteristic_notify_characteristic_client_configuration);
    }
}

/*******************************************************************************
**
** Function         app_ble_wifi_introducer_nw_security_callback
**
** Description      APP BLE wifi introducer NW Security callback.
**
** Returns          void
**
*******************************************************************************/
static void app_ble_wifi_introducer_nw_security_callback(tBSA_BLE_EVT event,
                  tBSA_BLE_MSG *p_data)
{
    if (event == BSA_BLE_SE_WRITE_EVT)
    {
        APP_INFO0("write security value");
    }
}

/*******************************************************************************
**
** Function         wifi_find_ap
**
** Description      fake function for Wifi Find AP
**
** Returns          void
**
*******************************************************************************/
static BOOLEAN wifi_find_ap(char *ssid_value)
{
    APP_INFO1("wifi_find_ap ssid : %s", ssid_value);
    return TRUE;
}

/*******************************************************************************
**
** Function         wifi_join
**
** Description      fake function for Wifi Join
**
** Returns          void
**
*******************************************************************************/

static BOOLEAN wifi_join(char *ssid_value, char *passphrase_value)
{
    APP_INFO1("wifi_join ssid : %s pass : %s return %d", ssid_value, passphrase_value, wifi_join_return_value);
    return wifi_join_return_value;
}

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
void app_ble_wifi_introducer_set_wifi_join_return_value_to_true(void)
{
    wifi_join_return_value = TRUE;
    APP_INFO1("app_ble_wifi_introducer_set_wifi_join_return_value_to_true : %d",
        wifi_join_return_value);
}

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
void app_ble_wifi_introducer_set_wifi_join_return_value_to_false(void)
{
    wifi_join_return_value = FALSE;
    APP_INFO1("app_ble_wifi_introducer_set_wifi_join_return_value_to_false : %d",
        wifi_join_return_value);
}

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
void app_ble_wifi_introducer_send_message(void)
{
    APP_DEBUG1("conn id : 0x%x",  app_ble_wifi_introducer_cb.conn_id);
    APP_DEBUG1("Client's Characteristic configuration : %d", 
        wifi_introducer_characteristic_notify_characteristic_client_configuration);
    /* If no client connectted or client has not registered for indication or notification, no action */
    if ((app_ble_wifi_introducer_cb.conn_id != BSA_BLE_INVALID_CONN) &&
        (wifi_introducer_characteristic_notify_characteristic_client_configuration &
          BSA_BLE_GATT_UUID_CHAR_CLIENT_CONFIG_ENABLE_NOTI))
    {
        APP_INFO0("Sending Notification");
        APP_INFO1("wifi_introducer_char_notify_value = %d", wifi_introducer_char_notify_value);
        if (app_ble_wifi_introducer_send_notification() == -1)
        {
            APP_ERROR0("Sent Notification Fail");
        }
    }
    else
    {
        APP_INFO0("No Action");
    }
    return;
}

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
void app_ble_wifi_introducer_display(void)
{
    APP_INFO0("***** WiFi Introducer Sensor *****");
    APP_INFO1("Device Name : %s", wifi_introducer_device_name);
    if (app_ble_wifi_introducer_cb.conn_id == BSA_BLE_INVALID_CONN)
    {
        APP_INFO0("No Client connected");
    }
    else
    {
        APP_INFO1("conn id : 0x%x",  app_ble_wifi_introducer_cb.conn_id);
    }
    APP_INFO1("Wifi Join Return Value : %d", wifi_join_return_value);
    APP_INFO1("Notify CCC : 0x%x",
        wifi_introducer_characteristic_notify_characteristic_client_configuration);
    APP_INFO1("Notify Value : %d", wifi_introducer_char_notify_value);
    APP_INFO1("Security Value : %d", wifi_introducer_char_nw_security_value);
    APP_INFO1("SSID : %s", wifi_introducer_char_nw_ssid_value);
    APP_INFO1("Passphrase : %s", wifi_introducer_char_nw_passphrase_value);
    APP_INFO1("Battery Level : %d", wifi_introducer_char_battery_level_value);
    APP_INFO1("Variables - wifi_introducer_ssid_name : %d, wifi_introducer_ssid_password : %d",
        wifi_introducer_ssid_name, wifi_introducer_ssid_password);
    return;
}

/*******************************************************************************
 **
 ** Function        app_ble_wifi_introducer_send_notification
 **
 ** Description     Send notification to client
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
static int app_ble_wifi_introducer_send_notification(void)
{
    tBSA_STATUS status;
    tBSA_BLE_SE_SENDIND ble_sendind_param;

    APP_INFO0("app_ble_wifi_introducer_send_notification");
    status = BSA_BleSeSendIndInit(&ble_sendind_param);
    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("BSA_BleSeSendIndInit failed status = %d", status);
        return -1;
    }

    ble_sendind_param.conn_id = app_ble_wifi_introducer_cb.conn_id;
    if (attr_index_notify >= APP_BLE_WIFI_INTRODUCER_ATTRIBUTE_MAX)
    {
        APP_ERROR0("Wrong attr_index_notify");
        return -1;
    }
    ble_sendind_param.attr_id = app_ble_wifi_introducer_cb.attr[attr_index_notify].attr_id;
    if (app_ble_wifi_introducer_cb.attr[attr_index_notify].val_len > BSA_BLE_SE_WRITE_MAX)
    {
        APP_ERROR1("Wrong Notification Value Length %d", app_ble_wifi_introducer_cb.attr[attr_index_notify].val_len);
        return -1;
    }
    ble_sendind_param.data_len = app_ble_wifi_introducer_cb.attr[attr_index_notify].val_len;
    memcpy(ble_sendind_param.value, app_ble_wifi_introducer_cb.attr[attr_index_notify].p_val, ble_sendind_param.data_len);
    ble_sendind_param.need_confirm = FALSE; // Notification

    APP_DUMP("send notification", (UINT8*)ble_sendind_param.value, ble_sendind_param.data_len);

    status = BSA_BleSeSendInd(&ble_sendind_param);
    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("BSA_BleSeSendInd failed status = %d", status);
        return -1;
    }

    return 0;
}

/*******************************************************************************
 **
 ** Function           app_ble_wifi_introducer_wifi_join_thread
 **
 ** Description       Wifi Join Thread
 **
 ** Returns            nothing
 **
 *******************************************************************************/
static void app_ble_wifi_introducer_wifi_join_thread(void)
{
    BOOLEAN result_wifi_join;
    app_lock_mutex(&join_mutex);
    do
    {
        APP_DEBUG0("running ");
        /* Wait for SSID & pass pharse to be configured */
        app_lock_mutex(&join_mutex);
        /*
        if ( ( wwd_wifi_is_ready_to_transceive( 0 ) == WWD_SUCCESS ) )
        {
           wiced_network_down( WICED_STA_INTERFACE );
        }
        */
        if (wifi_find_ap((char*)wifi_introducer_char_nw_ssid_value))
        {
            APP_INFO0("print_wifi_scan_result");
        }

        result_wifi_join = wifi_join((char*)wifi_introducer_char_nw_ssid_value, (char*)wifi_introducer_char_nw_passphrase_value);
        if (!result_wifi_join)
        {
             APP_INFO0("Join Failed!!!");
             //writing BLE notification
             wifi_introducer_char_notify_value = 0;
             wifi_introducer_ssid_name = FALSE;
             wifi_introducer_ssid_password = FALSE;
        }
        else
        {
            APP_INFO0("Join Succeeded!!!");
            //writing BLE notification
            wifi_introducer_char_notify_value = 1;
        }
        app_ble_wifi_introducer_send_message();
        wifi_introducer_ssid_name = FALSE;
        wifi_introducer_ssid_password = FALSE;
    }while(1);

    APP_DEBUG0("Exit");
    pthread_exit(NULL);
}

/*******************************************************************************
 **
 ** Function           app_ble_wifi_introducer_create_wifi_join_thread
 **
 ** Description       Create Wifi Join Thread
 **
 ** Returns            nothing
 **
 *******************************************************************************/
int app_ble_wifi_introducer_create_wifi_join_thread(void)
{
    int status = 0;
    status = app_create_thread(app_ble_wifi_introducer_wifi_join_thread, 0, 0,
        &join_thread);
    if (status < 0)
    {
        APP_ERROR1("create wifi join thread failed: %d", status);
        return -1;
    }
    return 0;
}

/*******************************************************************************
 **
 ** Description   dueros wifi introdecer function implementations
 **
 *******************************************************************************/
#ifdef DUEROS
static void dueros_set_device_name(void) {
    memset((char *)wifi_introducer_device_name, 0, BD_NAME_LEN + 1);
    sprintf((char *)wifi_introducer_device_name, "%s", "DuerOS_");
    app_get_mac_address((char *)wifi_introducer_device_name + sizeof("DuerOS_") - 1,
        5, "wlan0", NULL, 0);
    APP_DEBUG1("Bt Device Name: %s", (char *)wifi_introducer_device_name);
}

static int dueros_socket_send(char *msg, int len) {
    return socket_send(dueros_socket_fd, msg, len);
}

static void *dueros_socket_recieve(void *arg) {
    int bytes = 0;
    char data[DUEROS_SOCKET_RECV_LEN];
    int buf_size = app_ble_wifi_introducer_cb.attr[attr_index_notify].max_val_len;

    dueros_socket_fd = setup_socket_client(dueros_socket_path);
    if (dueros_socket_fd < 0) {
        APP_ERROR0("Fail to connect server socket\n");
        goto exit;
    }

    while (dueros_socket_done) {
        memset(data, 0, sizeof(data));
        bytes = socket_recieve(dueros_socket_fd, data, sizeof(data));
        if (bytes <= 0) {
            APP_DEBUG0("Server leaved, break\n");
            break;
        }

        //APP_DEBUG1("wifi introdecer socket recv len: %d\n", bytes);
        //APP_DUMP("recv data", (UINT8*)data, bytes);

        if (buf_size >= bytes) {
            app_ble_wifi_introducer_cb.attr[attr_index_notify].val_len = bytes;
            memcpy(app_ble_wifi_introducer_cb.attr[attr_index_notify].p_val, data, bytes);
            app_ble_wifi_introducer_send_message();
        } else {
            APP_ERROR1("Socket recv overflow, bytes: %d, buf_size: %d\n", bytes, buf_size);
        }
    }

exit:
    APP_DEBUG0("Exit dueros socket thread\n");
    pthread_exit(0);
}

static int dueros_socket_thread_create(void) {
    dueros_socket_done = 1;
    if (pthread_create(&dueros_tid, NULL, dueros_socket_recieve, NULL)) {
        APP_ERROR0("Create dueros socket thread failed\n");
        return -1;
    }

    return 0;
}

static void dueros_socket_thread_delete(void) {
    dueros_socket_done = 0;
    teardown_socket_client(dueros_socket_fd);

    if (dueros_tid) {
        pthread_join(dueros_tid, NULL);
        dueros_tid = 0;
    }
}

static void dueros_wifi_introducer_ssid_password_callback(tBSA_BLE_EVT event,
                  tBSA_BLE_MSG *p_data)
{
    int attr_index;
    int val_len;

    APP_INFO0("dueros_wifi_introducer_ssid_password_callback");

    if (event == BSA_BLE_SE_WRITE_EVT)
    {
	    attr_index = app_ble_wifi_introducer_find_attr_index_by_attr_id(p_data->ser_write.handle);
        if (attr_index < 0)
        {
            APP_ERROR0("Cannot find matched attr_id");
	        return;
        }

        val_len = app_ble_wifi_introducer_cb.attr[attr_index].val_len;
        if (val_len >= APP_BLE_WIFI_INTRODUCER_GATT_ATTRIBUTE_SIZE)
        {
            APP_ERROR0("Wrong wifi_introducer_char_nw_ssid_value length");
	        return;
        }
        dueros_characteristic_value[val_len] = 0;

        //APP_DUMP("callback write value", dueros_characteristic_value, val_len);
        dueros_socket_send((char*)dueros_characteristic_value, val_len);
    }
}

static void dueros_wifi_introducer_ccc_callback(tBSA_BLE_EVT event,
            tBSA_BLE_MSG *p_data)
{
    if (event == BSA_BLE_SE_WRITE_EVT)
    {
        APP_INFO0("dueros_wifi_introducer_ccc_callback");
        APP_INFO1("wifi_introducer_characteristic_notify_characteristic_client_configuration = %d" ,
            wifi_introducer_characteristic_notify_characteristic_client_configuration);
    }

    //dueros_socket_send((char*)&wifi_introducer_characteristic_notify_characteristic_client_configuration, sizeof(UINT16));
}

static int dueros_wifi_introducer_create_gatt_database(void)
{
    int srvc_attr_index;
    tBT_UUID service_uuid;
    tAPP_BLE_WIFI_INTRODUCER_ATTRIBUTE attr;

    APP_INFO0("dueros_wifi_introducer_create_gatt_database");

    service_uuid.len = LEN_UUID_16;
    service_uuid.uu.uuid16 = DUEROS_WIFI_CONFIG_SERVICE_UUID;
    srvc_attr_index = app_ble_wifi_introducer_create_service(&service_uuid, 20);
    if (srvc_attr_index < 0)
    {
        APP_ERROR0("DuerOS Wifi Config Service Create Fail");
        return -1;
    }

    memset(&attr, 0, sizeof(tAPP_BLE_WIFI_INTRODUCER_ATTRIBUTE));
    attr.attr_UUID.len = LEN_UUID_16;
    attr.attr_UUID.uu.uuid16 = DUEROS_WIFI_CONFIG_CHARACTERISTIC_UUID;
    attr.service_id = app_ble_wifi_introducer_cb.attr[srvc_attr_index].service_id;
    attr.perm = BSA_GATT_PERM_READ | BSA_GATT_PERM_WRITE; //17
    attr.prop = BSA_GATT_CHAR_PROP_BIT_READ | BSA_GATT_CHAR_PROP_BIT_WRITE |
                    BSA_GATT_CHAR_PROP_BIT_NOTIFY | BSA_GATT_CHAR_PROP_BIT_INDICATE |
                    BSA_GATT_CHAR_PROP_BIT_WRITE_NR;
    //attr.prop = BSA_GATT_CHAR_PROP_BIT_READ | BSA_GATT_CHAR_PROP_BIT_WRITE |
    //                BSA_GATT_CHAR_PROP_BIT_WRITE_NR; //14

    attr.attr_type = BSA_GATTC_ATTR_TYPE_CHAR;
    attr.val_len = strlen(DUEROS_CHARACTERISTIC_VALUE);
    attr.max_val_len = APP_BLE_WIFI_INTRODUCER_GATT_ATTRIBUTE_SIZE - 1; // reserve one byte for end of string
    attr.p_val = dueros_characteristic_value;
    attr.p_cback = dueros_wifi_introducer_ssid_password_callback;
    attr_index_notify = app_ble_wifi_introducer_add_char(&attr);

    memset(&attr, 0, sizeof(tAPP_BLE_WIFI_INTRODUCER_ATTRIBUTE));
    attr.attr_UUID.len = LEN_UUID_16;
    attr.attr_UUID.uu.uuid16 = DUEROS_WIFI_CONFIG_DESCRIPTOR_UUID;
    attr.service_id = app_ble_wifi_introducer_cb.attr[srvc_attr_index].service_id;
    attr.perm = BSA_GATT_PERM_READ | BSA_GATT_PERM_WRITE; //17
    attr.attr_type = BSA_GATTC_ATTR_TYPE_CHAR_DESCR;
    attr.val_len = attr.max_val_len = sizeof(UINT16);
    attr.p_val = &wifi_introducer_characteristic_notify_characteristic_client_configuration;
    attr.p_cback = dueros_wifi_introducer_ccc_callback;
    app_ble_wifi_introducer_add_char(&attr);

    return 0;
}
#endif

