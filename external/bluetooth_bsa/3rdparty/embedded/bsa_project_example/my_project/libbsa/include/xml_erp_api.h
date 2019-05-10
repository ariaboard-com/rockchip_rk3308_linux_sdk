/******************************************************************************
 **
 **  Name:          xml_erp_api.h
 **
 **
 **  Description:   This module contains xml parser of MAP event report
 **
**  Copyright (c) 2003-2014, Broadcom Corp., All Rights Reserved.
**  Broadcom Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/

#ifndef XML_ERP_API_H
#define XML_ERP_API_H

#include "bt_target.h"
#include "gki.h"
#include "xml_pars_api.h"


#ifndef XML_EVT_RPT_CARRY_OVER_LEN
#define XML_EVT_RPT_CARRY_OVER_LEN 512 /* number of bytes we can store in case we did not yet find the > */
#endif

/**********************************************************************************/
/* Make sure there is a pair of len and actual data */
typedef struct
{
    UINT16           type_len;
    UINT16           handle_len;
    UINT16           folder_len;
    UINT16           old_folder_len;
    UINT16           msg_type_len;
    UINT16           datetime_len;
    UINT16           subject_len;
    UINT16           sender_name_len;
    UINT16           priority_len;
    UINT16           cal_type_len;
    UINT16           summary_len;
    UINT16           update_len;
    UINT16           ori_name_len;
    UINT16           ori_addr_len;

    UINT8           *type;
    UINT8           *handle;
    UINT8           *folder;
    UINT8           *old_folder;
    UINT8           *msg_type;
    UINT8           *datetime;
    UINT8           *subject;
    UINT8           *sender_name;
    UINT8           *priority;
    UINT8           *cal_type;
    UINT8           *summary;
    UINT8           *update;
    UINT8           *ori_name;
    UINT8           *ori_addr;
} tXML_EVT_RPT_ENTRY;

/**********************************************************************************/

enum
{
    XML_EVT_RPT_OK, /* parsing is ok, operation is ok */
    XML_EVT_RPT_PENDING, /* parsing is ok but not enough data */
    XML_EVT_RPT_END_LIST, /* found </MAP-event-report> */
    XML_EVT_RPT_OUT_FULL, /* output buffer full  /MAP-event-report not reached! data is dumped */
    XML_EVT_RPT_ERROR,     /* some parsing error occured */
    XML_EVT_RPT_NO_RES,    /* ran out of resources (memory) */
    XML_EVT_RPT_DST_NO_RES /* ran out of destination data buffer */
} ;
typedef UINT8    tXML_EVT_RPT_RES;


typedef struct
{
    tXML_EVT_RPT_ENTRY     *p_entry;   /* point to  parser_cb->ilist.evt_rpt */
    tXML_PROP              *p_prop;    /* GKI_BUF3_SIZE */

    tXML_PROP              *offset_prop;    /* current filling property */
    UINT16                  prop_num;       /* number of properties left to be filled in */

    INT16                   current_entry;
    INT16                   max_name_len;    /* maximum length of name length of entry
                                                XML parser limits to 196 bytes i think. */
    UINT16                  max_entry;
    BOOLEAN                 ended;
    UINT16                  prop_index;
    UINT16                  max_num_prop;
    UINT8                   obj;           /* the XML object - (tFTC_XML_OBJ + 1)*/
} tXML_EVT_RPT_STATE;

typedef struct
{
    tXML_MUL_STATE           xml;
    tXML_EVT_RPT_STATE       xml_user_data;
} tXML_EVT_RPT_PARSER;

/* only this value is of significance, if not ok frame is dropped by parser */
#define XML_EVT_RPT_ENTRY_OK    0

typedef UINT8 tXML_EVT_RPT_STATUS;


typedef struct
{
    UINT8           *data;
    UINT16           len;
    BOOLEAN          final;     /* If TRUE, entry is last of the series */
    tXML_EVT_RPT_STATUS  status;    /* Fields are valid when status is BTA_FTC_OK */
} tXML_EVT_RPT_LIST; /* clone of tBTA_FTC_LIST */


/**************************************************************************************
** Function         XML_EvtRptInit
**
** Description      Initialize xml parser state machine.
**
** Parameters       p_xml_state: address of parser structure, allocate an additional space
**                                of size XML_EVT_RPT_CARRY_OVER_LEN right after p_xml_state
**                                to hold carry over data.
**                  p_entry    : points start of output directory entry. caller needs do free this memory
**                  max_entry  : max is 16 bit integer value which is the maximum number of folder entries.

**
** Returns          void
**************************************************************************************/
MCE_API void XML_EvtRptInit( tXML_EVT_RPT_PARSER  *p_xml_state,
                             tXML_EVT_RPT_ENTRY    *p_entry,
                             const UINT16             max_entry  );


/**************************************************************************************
** Function         XML_EvtRptParse
**
** Description      This function is called to parse the xml data received from OBEX
**                  into folder entries associated with properties value. It can also be
**                  used as clean up function to delete left over data from previous parse.
**                  This clean up function is typically used when application runs out of
**                  resource and decides to discard extra xml data received.
**
** Parameters       p_xml_state: pointer to a xml parser initialized by XML_FolderInit().
**                  xml_data: valid pointer to OBEX list data in xml format.
**                  xml_len: length of the  package, must be non-zero value.
**                  dst_data: valid pointer to the buffer for holding converted folder entry name.
**                            When dst_data is NULL, clean up all remaining data in the parser.
**                  dst_len: length of the dst_data buffer, its carry out value is the number
**                            of bytes of available buffer remains.
**                  num_entries: current number of entries, in the end it is the total number of entries
**
** Returns          tXML_EVT_RPT_RES (see xml_flp.h)
**                  XML_PENDING: parsing not completed
**                  XML_END_LIST: found /folder-listing but no final flag detected
**                  XML_EVT_RPT_OUT_FULL: reached max_entry -> do not call parser anymore!!! dump data
**                  XML_EVT_RPT_DST_NO_RES : run out of dst buffer resource while
**                                          some xml data still remains.

**************************************************************************************/
MCE_API extern tXML_EVT_RPT_RES XML_EvtRptParse( tXML_EVT_RPT_PARSER   *p_xml_state,
                                                 UINT8 *xml_data, UINT16 xml_len,
                                                 UINT8 *dst_data, UINT16 *dst_len,
                                                 UINT16               *num_entries );


#endif
