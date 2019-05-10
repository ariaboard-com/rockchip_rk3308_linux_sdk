/*****************************************************************************
**
**  Name:           app_avk.h
**
**  Description:    Bluetooth Audio/Video Streaming application
**
**  Copyright (c) 2009-2015, Broadcom Corp., All Rights Reserved.
**  Broadcom Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/

#ifndef APP_AVK_H_
#define APP_AVK_H_
#include "data_types.h"
#include "bsa_avk_api.h"

#ifdef PCM_ALSA
#include "alsa/asoundlib.h"
#define APP_AVK_ASLA_DEV "default"
#endif

#define APP_AVK_MAX_CONNECTIONS BSA_MAX_AVK_CONNECTIONS

#define BSA_MIN_ABS_VOLUME        0x00   /* Min and max absolute vol */
#define BSA_MAX_ABS_VOLUME        0x7F

/* Menu items */
enum
{
    /* avk recieve */
    APP_AVK_MENU_DISCOVERY = 1,
    APP_AVK_MENU_REGISTER,
    APP_AVK_MENU_DEREGISTER,
    APP_AVK_MENU_OPEN,
    APP_AVK_MENU_CLOSE, //5
    APP_AVK_MENU_PLAY_START,
    APP_AVK_MENU_PLAY_STOP,
    APP_AVK_MENU_PLAY_PAUSE,
    APP_AVK_MENU_VOLUME_UP,
    APP_AVK_MENU_VOLUME_DOWN, //10
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

/*
 * Types
 */
typedef struct
{
    BD_ADDR bda_connected;  /* peer BDA*/
    BOOLEAN in_use;         /* TRUE if this connection block is in use */
    int index;              /* Index of this connection block in the array */

    UINT8 ccb_handle;       /* AVK (A2DP) handle */
    BOOLEAN is_open;        /* TRUE if AVK (signaling channel) is open */

    UINT8 rc_handle;        /* AVRCP handle */
    BOOLEAN is_rc_open;     /* TRUE if AVRCP is open */
    tBSA_AVK_FEAT peer_features;       /* peer AVRCP features mask */
    UINT16 peer_version;    /* Peer AVRCP version */

    BOOLEAN is_streaming_chl_open; /* TRUE is streaming channel is open */
    BOOLEAN is_started_streaming;     /* TRUE if streaming has started */
    tBSA_AVK_CODEC format;
    UINT16 sample_rate;
    UINT8 num_channel;
    UINT8 bit_per_sample;

    UINT16 m_uiAddressedPlayer;
    UINT16 m_uidCounterAddrPlayer;
    UINT16   m_uid_counter;
    BOOLEAN m_bDeviceSupportBrowse;
    UINT16 m_uiBrowsedPlayer;
    UINT16 m_iCurrentFolderItemCount;

    volatile BOOLEAN m_bAbsVolumeSupported;
    UINT8 volChangeLabel;   /* label used for volume change registration. */

} tAPP_AVK_CONNECTION;


typedef struct
{
    volatile BOOLEAN open_pending; /* Indicate that there is an open pending (when BSA tries to connect to peer)*/
    BD_ADDR open_pending_bda; /* BDA of device BSA is trying to connect */
    tUIPC_CH_ID uipc_audio_channel;
    int fd;
    UINT8 fd_codec_type;
#ifdef PCM_ALSA
    snd_pcm_t *alsa_handle;
#endif
    UINT8 label;
    tAPP_AVK_CONNECTION connections[APP_AVK_MAX_CONNECTIONS];
    UINT8 volume;           /* system volume percentage used for absolute volume */
    tBSA_AVK_CBACK *p_Callback;
    tAPP_AVK_CONNECTION *pStreamingConn;
} tAPP_AVK_CB;


/*******************************************************************************
**
** Function         app_avk_init
**
** Description      Init AVK application
**
** Parameters       Application callback (if null, default will be used)
**
** Returns          0 if successful, error code otherwise
**
*******************************************************************************/
int app_avk_init(tBSA_AVK_CBACK pcb);

/*******************************************************************************
**
** Function         app_avk_end
**
** Description      This function is used to close application
**
** Returns          void
**
*******************************************************************************/
void app_avk_end(void);

/*******************************************************************************
**
** Function         app_avk_register
**
** Description      Register a new AV sink point
**
** Returns          0 succees
**
*******************************************************************************/
int app_avk_register(void);

/*******************************************************************************
**
** Function         app_avk_deregister
**
** Description      DeRegister an AV sink point
**
** Returns          void
**
*******************************************************************************/
void app_avk_deregister(void);

/*******************************************************************************
**
** Function         app_avk_open
**
** Description      Function to open AV connection
**
** Returns          void
**
*******************************************************************************/
void app_avk_open(void);

/*******************************************************************************
**
** Function         app_avk_close
**
** Description      Function to close AVK connection
**
** Returns          void
**
*******************************************************************************/
void app_avk_close(BD_ADDR bd_addr);

/*******************************************************************************
 **
 ** Function         app_avk_close_all
 **
 ** Description      Function to close all AVK connections
 **
 ** Returns          void
 **
 *******************************************************************************/
void app_avk_close_all();

/*******************************************************************************
**
** Function         app_avk_open_rc
**
** Description      Function to opens avrc controller connection. AVK should be open before opening rc.
**
** Returns          void
**
*******************************************************************************/
void app_avk_open_rc(BD_ADDR bd_addr);

/*******************************************************************************
**
** Function         app_avk_close_rc
**
** Description      Function to closes avrc controller connection.
**
** Returns          void
**
*******************************************************************************/
void app_avk_close_rc(UINT8 rc_handle);

/*******************************************************************************
**
** Function         app_avk_close_str
**
** Description      Function to closes a2dp stream connection.
**
** Returns          void
**
*******************************************************************************/
void app_avk_close_str(UINT8 ccb_handle);


/*******************************************************************************
**
** Function         app_avk_cancel
**
** Description      cancel connection command
**
** Returns          void
**
*******************************************************************************/
void app_avk_cancel(BD_ADDR bda);

/*******************************************************************************
 **
 ** Function         void app_avk_rc_send_cmd
 **
 ** Description      Example of send avrcp commands
 **
 ** Returns          void
 **
 *******************************************************************************/
void app_avk_rc_send_cmd(UINT8 command);

/*******************************************************************************
**
** Function         app_avk_volume_up
**
** Description      Example of volume up
**
** Returns          void
**
*******************************************************************************/
void app_avk_volume_up(UINT8 rc_handle);

/*******************************************************************************
**
** Function         app_avk_volume_up
**
** Description      Example of volume down
**
** Returns          void
**
*******************************************************************************/
void app_avk_volume_down(UINT8 rc_handle);

/*******************************************************************************
**
** Function         app_avk_play_start
**
** Description      Example of start steam play
**
** Returns          void
**
*******************************************************************************/
void app_avk_play_start(UINT8 rc_handle);

/*******************************************************************************
**
** Function         app_avk_play_stop
**
** Description      Example of stop steam play
**
** Returns          void
**
*******************************************************************************/
void app_avk_play_stop(UINT8 rc_handle);

/*******************************************************************************
**
** Function         app_avk_play_pause
**
** Description      Example of pause steam pause
**
** Returns          void
**
*******************************************************************************/
void app_avk_play_pause(UINT8 rc_handle);

/*******************************************************************************
**
** Function         app_avk_play_next_track
**
** Description      Example of play next track
**
** Returns          void
**
*******************************************************************************/
void app_avk_play_next_track(UINT8 rc_handle);

/*******************************************************************************
**
** Function         app_avk_play_previous_track
**
** Description      Example of play previous track
**
** Returns          void
**
*******************************************************************************/
void app_avk_play_previous_track(UINT8 rc_handle);

/*******************************************************************************
**
** Function         app_avk_rc_cmd
**
** Description      Example of function to open AVK connection
**
** Returns          void
**
*******************************************************************************/
void app_avk_rc_cmd(UINT8 rc_handle);

/*******************************************************************************
**
** Function         app_avk_rc_send_command
**
** Description      Example of Send a RemoteControl command
**
** Returns          void
**
*******************************************************************************/
void app_avk_rc_send_command(UINT8 key_state, UINT8 command, UINT8 rc_handle);

/*******************************************************************************
**
** Function         app_avk_rc_send_click
**
** Description      Send press and release states of a command
**
** Returns          void
**
*******************************************************************************/
void app_avk_rc_send_click(UINT8 command, UINT8 rc_handle);

/*******************************************************************************
**
** Function         app_avk_send_get_element_attributes_cmd
**
** Description      Example of function to send Vendor Dependent Command
**
** Returns          void
**
*******************************************************************************/
void app_avk_send_get_element_attributes_cmd(UINT8 rc_handle);

/*******************************************************************************
 **
 ** Function         app_avk_rc_get_element_attr_command
 **
 ** Description      Example of function to send get_element_attr Command
 **
 ** Returns          void
 **
 *******************************************************************************/
void app_avk_rc_get_element_attr_command(UINT8 rc_handle);

/*******************************************************************************
 **
 ** Function         app_avk_rc_list_player_attr_command
 **
 ** Description      Example of function to send list_player_attr Command
 **
 ** Returns          void
 **
 *******************************************************************************/
void app_avk_rc_list_player_attr_command(UINT8 rc_handle);

/*******************************************************************************
 **
 ** Function         app_avk_rc_list_player_attr_value_command
 **
 ** Description      Example of function to send list_player_attr value Command
 **
 ** Returns          void
 **
 *******************************************************************************/
void app_avk_rc_list_player_attr_value_command(UINT8 attr, UINT8 rc_handle);

/*******************************************************************************
 **
 ** Function         app_avk_rc_get_player_value_command
 **
 ** Description      Example of get_player_value_command command
 **
 ** Returns          void
 **
 *******************************************************************************/
void app_avk_rc_get_player_value_command(UINT8 *attrs, UINT8 num_attr, UINT8 rc_handle);

/*******************************************************************************
 **
 ** Function         app_avk_rc_set_player_value_command
 **
 ** Description      Example of set_player_value_command command
 **
 ** Returns          void
 **
 *******************************************************************************/
void app_avk_rc_set_player_value_command(UINT8 num_attr, UINT8 *attr_ids, UINT8 * attr_val, UINT8 rc_handle);

/*******************************************************************************
 **
 ** Function         app_avk_rc_get_play_status_command
 **
 ** Description      Example of get_play_status
 **
 ** Returns          void
 **
 *******************************************************************************/
void app_avk_rc_get_play_status_command(UINT8 rc_handle);

/*******************************************************************************
 **
 ** Function         app_avk_rc_set_browsed_player_command
 **
 ** Description      Example of set_browsed_player
 **
 ** Returns          void
 **
 *******************************************************************************/
void app_avk_rc_set_browsed_player_command(UINT16  player_id, UINT8 rc_handle);

/*******************************************************************************
 **
 ** Function         app_avk_rc_set_addressed_player_command
 **
 ** Description      Example of set_addressed_player
 **
 ** Returns          void
 **
 *******************************************************************************/
void app_avk_rc_set_addressed_player_command(UINT16  player_id, UINT8 rc_handle);


/*******************************************************************************
 **
 ** Function         app_avk_rc_change_path_command
 **
 ** Description      Example of change_path
 **
 ** Returns          void
 **
 *******************************************************************************/
void app_avk_rc_change_path_command(UINT16   uid_counter, UINT8  direction, tAVRC_UID folder_uid, UINT8 rc_handle);


/*******************************************************************************
 **
 ** Function         app_avk_rc_get_folder_items
 **
 ** Description      Example of get_folder_items
 **
 ** Returns          void
 **
 *******************************************************************************/
void app_avk_rc_get_folder_items(UINT8  scope, UINT32  start_item, UINT32  end_item, UINT8 rc_handle);

/*******************************************************************************
 **
 ** Function         app_avk_rc_get_items_attr
 **
 ** Description      Example of get_items_attr
 **
 ** Returns          void
 **
 *******************************************************************************/
void app_avk_rc_get_items_attr(UINT8  scope, tAVRC_UID  uid, UINT16  uid_counter, UINT8 rc_handle);

/*******************************************************************************
 **
 ** Function         app_avk_rc_play_item
 **
 ** Description      Example of play_item
 **
 ** Returns          void
 **
 *******************************************************************************/
void app_avk_rc_play_item(UINT8  scope, tAVRC_UID  uid, UINT16  uid_counter, UINT8 rc_handle);


/*******************************************************************************
 **
 ** Function         app_avk_rc_add_to_play
 **
 ** Description      Example of add_to_play
 **
 ** Returns          void
 **
 *******************************************************************************/
void app_avk_rc_add_to_play(UINT8  scope, tAVRC_UID  uid, UINT16  uid_counter, UINT8 rc_handle);


/*******************************************************************************
**
** Function         avk_is_open
**
** Description      Check if AVK is open
**
** Parameters       BDA of device to check
**
** Returns          TRUE if AVK is open, FALSE otherwise.
**
*******************************************************************************/
BOOLEAN avk_is_open(BD_ADDR bda);

/*******************************************************************************
**
** Function         avk_is_any_open
**
** Description      Check if any AVK is open
**
** Parameters
**
** Returns          TRUE if AVK is open, FALSE otherwise. Return BDA of the connected device
**
*******************************************************************************/
BOOLEAN avk_is_any_open(BD_ADDR bda /* out */);

/*******************************************************************************
**
** Function         avk_is_rc_open
**
** Description      Check if AVRC is open
**
** Parameters
**
** Returns          TRUE if AVRC is open, FALSE otherwise.
**
*******************************************************************************/
BOOLEAN avk_is_rc_open(BD_ADDR bda);

/*******************************************************************************
**
** Function         avk_is_open_pending
**
** Description      Check if AVK Open is pending
**
** Parameters
**
** Returns          TRUE if open is pending, FALSE otherwise
**
*******************************************************************************/
BOOLEAN avk_is_open_pending();


/*******************************************************************************
**
** Function         avk_set_open_pending
**
** Description      Set AVK open pending
**
** Parameters
**
** Returns          void
**
*******************************************************************************/
void avk_set_open_pending(BOOLEAN bopenpend);

/*******************************************************************************
 **
 ** Function         app_avk_send_get_capabilities
 **
 ** Description      Sample code for attaining capability for events
 **
 ** Returns          void
 **
 *******************************************************************************/
void app_avk_send_get_capabilities(UINT8 rc_handle);

/*******************************************************************************
 **
 ** Function         app_avk_send_register_notification
 **
 ** Description      Sample code for attaining capability for events
 **
 ** Returns          void
 **
 *******************************************************************************/
void app_avk_send_register_notification(int evt, UINT8 rc_handle);


/*******************************************************************************
 **
 ** Function         app_avk_add_connection
 **
 ** Description      This function adds a connection
 **
 ** Returns          Pointer to the structure or NULL
 **
 *******************************************************************************/
tAPP_AVK_CONNECTION *app_avk_add_connection(BD_ADDR bd_addr);

/*******************************************************************************
 **
 ** Function         app_avk_reset_connection
 **
 ** Description      This function resets a connection
 **
 **
 *******************************************************************************/
void app_avk_reset_connection(BD_ADDR bd_addr);

/*******************************************************************************
 **
 ** Function         app_avk_find_connection_by_av_handle
 **
 ** Description      This function finds the connection structure by its handle
 **
 ** Returns          Pointer to the found structure or NULL
 **
 *******************************************************************************/
tAPP_AVK_CONNECTION *app_avk_find_connection_by_av_handle(UINT8 handle);

/*******************************************************************************
 **
 ** Function         app_avk_find_connection_by_rc_handle
 **
 ** Description      This function finds the connection structure by its handle
 **
 ** Returns          Pointer to the found structure or NULL
 **
 *******************************************************************************/
tAPP_AVK_CONNECTION *app_avk_find_connection_by_rc_handle(UINT8 handle);

/*******************************************************************************
 **
 ** Function         app_avK_find_connection_by_bd_addr
 **
 ** Description      This function finds the connection structure by its handle
 **
 ** Returns          Pointer to the found structure or NULL
 **
 *******************************************************************************/
tAPP_AVK_CONNECTION *app_avk_find_connection_by_bd_addr(BD_ADDR bd_addr);


/*******************************************************************************
 **
 ** Function         app_avk_find_streaming_connection
 **
 ** Description      This function finds the connection structure that is streaming
 **
 ** Returns          Pointer to the found structure or NULL
 **
 *******************************************************************************/
tAPP_AVK_CONNECTION *app_avk_find_streaming_connection();

/*******************************************************************************
 **
 ** Function         app_avk_num_connections
 **
 ** Description      This function number of connections
 **
 ** Returns          number of connections
 **
 *******************************************************************************/
int app_avk_num_connections();

/*******************************************************************************
 **
 ** Function         app_avk_find_connection_by_index
 **
 ** Description      This function finds the connection structure by its index
 **
 ** Returns          Pointer to the found structure or NULL
 **
 *******************************************************************************/
tAPP_AVK_CONNECTION *app_avk_find_connection_by_index(int index);

/*******************************************************************************
 **
 ** Function         app_avk_pause_other_streams
 **
 ** Description      This function pauses other streams when a new stream start
 **                  is received from device identified by bd_addr
 **
 ** Returns          None
 **
 *******************************************************************************/
void app_avk_pause_other_streams(BD_ADDR bd_addr);

/*******************************************************************************
 **
 ** Function         app_avk_set_abs_vol_rsp
 **
 ** Description      This function sends abs vol response
 **
 ** Returns          None
 **
 *******************************************************************************/
void app_avk_set_abs_vol_rsp(UINT8 volume, UINT8 rc_handle, UINT8 label);


/*******************************************************************************
 **
 ** Function         app_avk_reg_notfn_rsp
 **
 ** Description      This function sends reg notfn response
 **
 ** Returns          none
 **
 *******************************************************************************/
void app_avk_reg_notfn_rsp(UINT8 volume, UINT8 rc_handle, UINT8 label, UINT8 event, tBSA_AVK_CODE code);


/*******************************************************************************
 **
 ** Function         app_avk_get_label
 **
 ** Description      get transaction label (used to distinguish transactions)
 **
 ** Returns          UINT8
 **
 *******************************************************************************/
UINT8 app_avk_get_label();

/*******************************************************************************
 **
 ** Function         app_avk_display_connections
 **
 ** Description      This function displays the connections
 **
 ** Returns          status
 **
 *******************************************************************************/
void app_avk_display_connections(void);

/*******************************************************************************
 **
 ** Function         app_avk_send_delay_report
 **
 ** Description      Sample code to send delay report
 **
 ** Returns          void
 **
 *******************************************************************************/
void app_avk_send_delay_report(UINT16 delay);

/*******************************************************************************
 **
 ** Function         app_avk_socket_send
 **
 ** Description      Send the message through socket
 **
 ** Returns          int
 **
 *******************************************************************************/
int app_avk_socket_send(int cmd);
#endif /* APP_AVK_H_ */
