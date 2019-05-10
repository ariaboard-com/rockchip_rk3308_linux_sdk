/******************************************************************************
 **
 **  Name:          xml_mlp_api.h
 **           $Id:
 **
 **  Description:   This module contains xml parser of MAP message list object
 **
 **  Copyright (c) 2004-2005, Broadcom Corporation, All Rights Reserved.
 **  WIDCOMM Bluetooth Core. Proprietary and confidential.
 ******************************************************************************/

#ifndef XML_MLP_API_H
#define XML_MLP_API_H

#include "bt_target.h"
#include "gki.h"
#include "xml_pars_api.h"


#ifndef XML_ML_CARRY_OVER_LEN
#define XML_ML_CARRY_OVER_LEN 512 /* number of bytes we can store in case we did not yet find the > */
#endif

/**********************************************************************************/

typedef struct
{
    UINT16           msg_handle_len;
    UINT16           subject_len;
    UINT16           datetime_len;
    UINT16           sender_name_len;
    UINT16           sender_addressing_len;
    UINT16           replyto_addressing_len;
    UINT16           recipient_name_len;
    UINT16           recipient_addressing_len;
    UINT16           type_len;
    UINT16           org_msg_size_len;
    UINT16           text_len;
    UINT16           reception_status_len;
    UINT16           attachment_size_len;
    UINT16           priority_status_len;
    UINT16           read_len;
    UINT16           sent_len;
    UINT16           is_protected_len;
    UINT8           *msg_handle;
    UINT8           *subject;
    UINT8           *datetime;
    UINT8           *sender_name;
    UINT8           *sender_addressing;
    UINT8           *replyto_addressing;
    UINT8           *recipient_name;
    UINT8           *recipient_addressing;
    UINT8           *type;
    UINT8           *org_msg_size;
    UINT8           *text;
    UINT8           *reception_status;
    UINT8           *attachment_size;
    UINT8           *priority_status;
    UINT8           *read;
    UINT8           *sent;
    UINT8           *is_protected;
} tXML_ML_ENTRY;

/**********************************************************************************/

enum
{
    XML_ML_OK, /* parsing is ok, operation is ok */
    XML_ML_PENDING, /* parsing is ok but not enough data */
    XML_ML_END_LIST, /* found </MAP-msg-listing> */
    XML_ML_OUT_FULL, /* output buffer full  /MAP-msg-listing not reached! data is dumped */
    XML_ML_ERROR,     /* some parsing error occured */
    XML_ML_NO_RES,    /* ran out of resources (memory) */
    XML_ML_DST_NO_RES /* ran out of destination data buffer */
};
typedef UINT8    tXML_ML_RES;


typedef struct
{
    tXML_ML_ENTRY     *p_entry;   /* point to  parser_cb->mlist.evt_rpt */
    tXML_PROP         *p_prop;    /* GKI_BUF3_SIZE */

    tXML_PROP         *offset_prop;    /* current filling property */
    UINT16             prop_num;       /* number of properties left to be filled in */

    INT16              current_entry;
    INT16              max_name_len;    /* maximum length of name length of entry
                                           XML parser limits to 196 bytes i think. */
    UINT16             max_entry;
    BOOLEAN            ended;
    UINT16             prop_index;
    UINT16             max_num_prop;
    UINT8              obj;           /* the XML object - (tFTC_XML_OBJ + 1)*/
} tXML_ML_STATE;

typedef struct
{
    tXML_MUL_STATE    xml;
    tXML_ML_STATE     xml_user_data;
} tXML_ML_PARSER;

/* only this value is of significance, if not ok frame is dropped by parser */
#define XML_ML_ENTRY_OK    0

typedef UINT8 tXML_ML_STATUS;


typedef struct
{
    UINT8           *data;
    UINT16           len;
    BOOLEAN          final;     /* If TRUE, entry is last of the series */
    tXML_ML_STATUS   status;    /* Fields are valid when status is BTA_FTC_OK */
} tXML_ML_LIST; /* clone of tBTA_FTC_LIST */


/**************************************************************************************
** Function         XML_MlInit
**
** Description      Initialize xml parser state machine.
**
** Parameters       p_xml_state: address of parser structure, allocate an additional space
**                                of size XML_ML_CARRY_OVER_LEN right after p_xml_state
**                                to hold carry over data.
**                  p_entry    : points start of output directory entry. caller needs do free this memory
**                  max_entry  : max is 16 bit integer value which is the maximum number of folder entries.

**
** Returns          void
**************************************************************************************/
MCE_API void XML_MlInit( tXML_ML_PARSER  *p_xml_state,
                         tXML_ML_ENTRY   *p_entry,
                         const UINT16    max_entry  );


/**************************************************************************************
** Function         XML_MLParse
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
** Returns          tXML_ML_RES (see xml_flp.h)
**                  XML_PENDING: parsing not completed
**                  XML_END_LIST: found /folder-listing but no final flag detected
**                  XML_ML_OUT_FULL: reached max_entry -> do not call parser anymore!!! dump data
**                  XML_ML_DST_NO_RES : run out of dst buffer resource while
**                                          some xml data still remains.

**************************************************************************************/
MCE_API extern tXML_ML_RES XML_MlParse( tXML_ML_PARSER   *p_xml_state,
                                        UINT8 *xml_data, UINT16 xml_len,
                                        UINT8 *dst_data, UINT16 *dst_len,
                                        UINT16  *num_entries );


#endif
