/*****************************************************************************
 **
 **  Name:           app_dg.h
 **
 **  Description:    Bluetooth Manager application
 **
 **  Copyright (c) 2011-2012, Broadcom Corp., All Rights Reserved.
 **  Broadcom Bluetooth Core. Proprietary and confidential.
 **
 *****************************************************************************/

#ifndef APP_DG_H
#define APP_DG_H
#include "app_disc.h"

/*******************************************************************************
 **
 ** Function         app_dg_init
 **
 ** Description      Init DG application
 **
 ** Returns          status
 **
 *******************************************************************************/
int app_dg_init(void);

/*******************************************************************************
 **
 ** Function         app_dg_con_free_all
 **
 ** Description      Function to free all DG connection structure
 **
 ** Returns          void
 **
 *******************************************************************************/
void app_dg_con_free_all(void);

/*******************************************************************************
 **
 ** Function         app_dg_start
 **
 ** Description      Start DG application
 **
 ** Returns          status
 **
 *******************************************************************************/
int app_dg_start(void);

/*******************************************************************************
 **
 ** Function         app_dg_stop
 **
 ** Description      Stop DG application
 **
 ** Returns          status
 **
 *******************************************************************************/
int app_dg_stop(void);

/*******************************************************************************
 **
 ** Function         app_dg_open
 **
 ** Description      Example of function to open a DG connection
 **
 ** Returns          int
 **
 *******************************************************************************/
int app_dg_open();

/*******************************************************************************
 **
 ** Function         app_dg_open_ex
 **
 ** Description      Example of function to open a DG connection for multiple
 **                  or single connections.
 **
 ** Returns          void
 **
 *******************************************************************************/
int app_dg_open_ex(int single_conn_only);

/*******************************************************************************
 **
 ** Function         app_dg_close
 **
 ** Description      Example of function to close a DG connection
 **
 ** Returns          int
 **
 *******************************************************************************/
int  app_dg_close(int connection);

/*******************************************************************************
 **
 ** Function         app_dg_listen
 **
 ** Description      Example of function to start an SPP server
 **
 ** Returns          status
 **
 *******************************************************************************/
int app_dg_listen(void);

/*******************************************************************************
 **
 ** Function         app_dg_shutdown
 **
 ** Description      Example of function to stop an SPP server
 **
 ** Returns          status
 **
 *******************************************************************************/
int app_dg_shutdown(int connection);

/*******************************************************************************
 **
 ** Function         app_dg_read
 **
 ** Description      Example of function to read a data gateway port
 **
 ** Parameters
 **
 ** Returns          void
 **
 *******************************************************************************/
int app_dg_read(int connection);


/*******************************************************************************
 **
 ** Function         app_dg_send_file
 **
 ** Description      Example of function to send a file
 **
 ** Returns          void
 **
 *******************************************************************************/
int app_dg_send_file(char * p_file_name, int connection);


/*******************************************************************************
 **
 ** Function         app_dg_send_data
 **
 ** Description      Example of function to send data
 **
 ** Returns          void
 **
 *******************************************************************************/
int app_dg_send_data(int connection);


/*******************************************************************************
 **
 ** Function         app_dg_find_service
 **
 ** Description      Example of function to find custom bluetooth service
 **
 ** Returns          void
 **
 *******************************************************************************/
int app_dg_find_service(void);

/*******************************************************************************
 **
 ** Function         app_dg_get_service_list
 **
 ** Description      Example of function to get service list of peer device
 **
 ** Returns          void
 **
 *******************************************************************************/
int app_dg_get_service_list(void);

/*******************************************************************************
 **
 ** Function         app_dg_loopback_toggle
 **
 ** Description      This function is used to stop DG
 **
 ** Returns          void
 **
 *******************************************************************************/
void app_dg_loopback_toggle(void);

/*******************************************************************************
 **
 ** Function         app_dg_con_display
 **
 ** Description      Function to display DG connection status
 **
 ** Returns          void
 **
 *******************************************************************************/
void app_dg_con_display(void);

/*******************************************************************************
 **
 ** Function         app_dg_get_service_name
 **
 ** Description      Example of function to get service name of peer device
 **
 ** Returns          void
 **
 *******************************************************************************/
int app_dg_get_service_name(void);

/*******************************************************************************
 **
 ** Function         app_dg_disc_cback
 **
 ** Description      discovery callback for app_dg
 **
 ** Returns          none
 **
 *******************************************************************************/
void app_dg_disc_cback(tBSA_DISC_EVT event, tBSA_DISC_MSG *p_data);

/*******************************************************************************
 **
 ** Function         app_dg_get_service_list_with_dev
 **
 ** Description      Example of function to get service list of peer device
 **
 ** Returns          void
 **
 *******************************************************************************/
int app_dg_get_service_list_with_dev(UINT8 device);

/*******************************************************************************
 **
 ** Function         app_dg_disc_and_get_service_list
 **
 ** Description      Trigger discovery and get service list
 **
 ** Returns          none
 **
 *******************************************************************************/
void app_dg_disc_and_get_service_list();

#endif

