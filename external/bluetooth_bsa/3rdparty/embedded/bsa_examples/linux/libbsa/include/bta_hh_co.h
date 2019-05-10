/*****************************************************************************
**
**  Name:           bta_hh_co.h
**
**  Description:    This is the interface file for hid host call-out
**                  functions.
**
**  Copyright (c) 2005 - 2014, Broadcom Corp., All Rights Reserved.
**  Widcomm Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/
#ifndef BTA_HH_CO_H
#define BTA_HH_CO_H

#include "bta_hh_api.h"

typedef struct
{
    UINT16              rpt_uuid;
    UINT8               rpt_id;
    tBTA_HH_RPT_TYPE    rpt_type;
    UINT8               inst_id;
    UINT8               prop;
}tBTA_HH_RPT_CACHE_ENTRY;

/*******************************************************************************
**
** Function         bta_hh_co_data
**
** Description      This callout function is executed by HH when data is received
**                  in interupt channel.
**
**
** Returns          void.
**
*******************************************************************************/
BTA_API extern void bta_hh_co_data(UINT8 dev_handle, UINT8 *p_rpt, UINT16 len,
                                   tBTA_HH_PROTO_MODE mode, UINT8 sub_class,
                                   UINT8 ctry_code, BD_ADDR peer_addr, UINT8 app_id);

/*******************************************************************************
**
** Function         bta_hh_co_open
**
** Description      This callout function is executed by HH when connection is
**                  opened, and application may do some device specific
**                  initialization.
**
** Returns          void.
**
*******************************************************************************/
/* BSA_SPECIFIC */
BTA_API extern void bta_hh_co_open(UINT8 dev_handle, UINT8 sub_class,
                                   UINT16 attr_mask, UINT8 app_id, BD_ADDR addr);

/*******************************************************************************
**
** Function         bta_hh_co_close
**
** Description      This callout function is executed by HH when connection is
**                  closed, and device specific finalizatio nmay be needed.
**
** Returns          void.
**
*******************************************************************************/
BTA_API extern void bta_hh_co_close(UINT8 dev_handle, UINT8 app_id);

#if (BLE_INCLUDED == TRUE && BTA_HH_LE_INCLUDED == TRUE)
/*******************************************************************************
**
** Function         bta_hh_le_co_rpt_info
**
** Description      This callout function is to convey the report information on
**                  a HOGP device to the application. Application can save this
**                  information in NV if device is bonded and load it back when
**                  stack reboot.
**
** Parameters       remote_bda  - remote device address
**                  rpt_id : report ID
**                  rpt_type: report type
**                  rpt_uuid: report UUID
**                  prop: report property
**                  inst_id: service and charactersitic instance ID
**                  app_id      - application id
**
** Returns          void.
**
*******************************************************************************/
BTA_API extern void bta_hh_le_co_rpt_info(BD_ADDR remote_bda,
                                          tBTA_HH_RPT_CACHE_ENTRY *p_entry,
                                          UINT8 app_id);


/*******************************************************************************
**
** Function         bta_hh_le_co_cache_load
**
** Description      This callout function is to request the application to load the
**                  cached HOGP report if there is any. When cache reading is completed,
**                  bta_hh_le_ci_cache_load() is called by the application.
**
** Parameters       remote_bda  - remote device address
**                  p_num_rpt: number of cached report
**                  app_id      - application id
**
** Returns          the acched report array
**
*******************************************************************************/
BTA_API extern tBTA_HH_RPT_CACHE_ENTRY *bta_hh_le_co_cache_load (BD_ADDR remote_bda,
                                                                 UINT8 *p_num_rpt,
                                                                 UINT8 app_id);

/*******************************************************************************
**
** Function         bta_hh_le_co_reset_rpt_cache
**
** Description      This callout function is to reset the HOGP device cache.
**
** Parameters       remote_bda  - remote device address
**
** Returns          none
**
*******************************************************************************/
BTA_API extern void bta_hh_le_co_reset_rpt_cache (BD_ADDR remote_bda, UINT8 app_id);

#endif /* #if (BLE_INCLUDED == TRUE && BTA_HH_LE_INCLUDED == TRUE) */
#endif /* BTA_HH_CO_H */

