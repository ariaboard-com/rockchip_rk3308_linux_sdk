/*****************************************************************************/
/*                                                                           */
/*  Name:          btu.h                                                     */
/*                                                                           */
/*  Description:   this file contains the main Bluetooth Upper Layer         */
/*                 definitions. The Widcomm implementations of L2CAP         */
/*                 RFCOMM, SDP and the BTIf run as one GKI task. The         */
/*                 btu_task switches between them.                           */
/*                                                                           */
/*  Copyright (c) 1999-2014, Broadcom Corp., All Rights Reserved.            */
/*  Broadcom Bluetooth Core. Proprietary and confidential.                   */
/*****************************************************************************/
#ifndef BTU_H
#define BTU_H

#include "bt_target.h"
#include "gki.h"
#if (defined(HCILP_INCLUDED) && HCILP_INCLUDED == TRUE)
#include "hcilp.h"
#endif
#include "hciutil.h"

/* Define the BTU mailbox usage
*/
#define BTU_HCI_RCV_MBOX        TASK_MBOX_0     /* Messages from HCI  */
#define BTU_BTIF_MBOX           TASK_MBOX_1     /* Messages to BTIF   */


/* callbacks
*/
typedef void (*tBTU_TIMER_CALLBACK)(TIMER_LIST_ENT *p_tle);
typedef void (*tBTU_EVENT_CALLBACK)(BT_HDR *p_hdr);


/* Define the timer types maintained by BTU
*/
#define BTU_TTYPE_BTM_DEV_CTL       1
#define BTU_TTYPE_L2CAP_LINK        2
#define BTU_TTYPE_L2CAP_CHNL        3
#define BTU_TTYPE_L2CAP_HOLD        4
#define BTU_TTYPE_SDP               5
#define BTU_TTYPE_BTM_SCO           6
#define BTU_TTYPE_BTM_ACL           7
#define BTU_TTYPE_BTM_RMT_NAME      8
#define BTU_TTYPE_RFCOMM_MFC        9
#define BTU_TTYPE_RFCOMM_PORT       10
#define BTU_TTYPE_AUTO_SYNC         11
#define BTU_TTYPE_SPP_CONN_RETRY    12
#define BTU_TTYPE_USER_FUNC         13

#define BTU_TTYPE_FTP_DISC          14
#define BTU_TTYPE_OPP_DISC          15

#define BTU_TTYPE_IPFRAG_TIMER      16
#define BTU_TTYPE_HSP2_AT_CMD_TO    17
#define BTU_TTYPE_HSP2_REPEAT_RING  18

#define BTU_TTYPE_HSP2_WAIT_OK      19

/* BNEP Timers */
#define BTU_TTYPE_BNEP              20

/* OBX */
#define BTU_TTYPE_OBX_CLIENT_TO     21
#define BTU_TTYPE_OBX_SERVER_TO     22
#define BTU_TTYPE_OBX_SVR_SESS_TO   23


#define BTU_TTYPE_HSP2_SDP_FAIL_TO  24
#define BTU_TTYPE_HSP2_SDP_RTRY_TO  25

#define BTU_TTYPE_AVDT_CCB_RET      26
#define BTU_TTYPE_AVDT_CCB_RSP      27
#define BTU_TTYPE_AVDT_CCB_IDLE     28
#define BTU_TTYPE_AVDT_SCB_TC       29

#define BTU_TTYPE_HID_DEV_REPAGE_TO 30
#define BTU_TTYPE_HID_HOST_REPAGE_TO 31

#define BTU_TTYPE_HSP2_DELAY_CKPD_RCV 32

#define BTU_TTYPE_SAP_TO            33

#define BTU_TTYPE_SLIP_RETRANSMIT_TO 34

#define BTU_TTYPE_SLIP_DELAY_ACK_TO  35

#define BTU_TTYPE_SLIP_CONTROL_TO    36
/* BPP Timer */
#define BTU_TTYPE_BPP_REF_CHNL     37

/* LP HC idle Timer */
#define BTU_TTYPE_LP_HC_IDLE_TO 38

/* Patch RAM Timer */
#define BTU_TTYPE_PATCHRAM_TO 39

#define BTU_TTYPE_SLIP_FLOW_CONTROL_TO    40

/* Dual Stack btm_sync state machine timer */
#define BTU_TTYPE_BTM_SYNC_TO       41

/* eL2CAP Info Request and other proto cmds timer */
#define BTU_TTYPE_L2CAP_FCR_ACK     42
#define BTU_TTYPE_L2CAP_INFO        43

/* BTU internal for BR/EDR HCI command timeout */
#define BTU_TTYPE_BTU_CMD_CMPL                      44

/* H4IBSS general Timer */
#define BTU_TTYPE_H4IBSS_TO                         45

#define BTU_TTYPE_MCA_CCB_RSP                       46

/* BTU internal timer for BLE activity */
#define BTU_TTYPE_BLE_INQUIRY                       47
#define BTU_TTYPE_BLE_GAP_LIM_DISC                  48
#define BTU_TTYPE_ATT_WAIT_FOR_RSP                  49
#define BTU_TTYPE_SMP_PAIRING_CMD                   50
#define BTU_TTYPE_BLE_RANDOM_ADDR                   51
#define BTU_TTYPE_ATT_WAIT_FOR_APP_RSP              52
#define BTU_TTYPE_ATT_WAIT_FOR_IND_ACK              53
#define BTU_TTYPE_BLE_GAP_FAST_ADV                  55
#define BTU_TTYPE_BLE_OBSERVE                       56
#define BTU_TTYPE_L2CAP_LE_CHNL                     57

#define BTU_TTYPE_UCD_TO                            58

/* BTU timer event for TBFC */
#define BTU_TTYPE_TBFC_RESUME                       59


/* BTU timer event for AVRC commands */
#define BTU_TTYPE_AVRC_CMD                          60

/* Define the BTU_TASK APPL events
*/
#if (defined(NFC_SHARED_TRANSPORT_ENABLED) && (NFC_SHARED_TRANSPORT_ENABLED==TRUE))
#define BTU_NFC_AVAILABLE_EVT   EVENT_MASK(APPL_EVT_0)  /* Notifies BTU task that NFC is available (used for shared NFC+BT transport) */
#endif

/* This is the inquiry response information held by BTU, and available
** to applications.
*/
typedef struct
{
    BD_ADDR     remote_bd_addr;
    UINT8       page_scan_rep_mode;
    UINT8       page_scan_per_mode;
    UINT8       page_scan_mode;
    DEV_CLASS   dev_class;
    UINT16      clock_offset;
} tBTU_INQ_INFO;



#define BTU_MAX_REG_TIMER     (2)   /* max # timer callbacks which may register */
#define BTU_MAX_REG_EVENT     (6)   /* max # event callbacks which may register */
#define BTU_DEFAULT_DATA_SIZE (0x2a0)

#if (BLE_INCLUDED == TRUE)
#define BTU_DEFAULT_BLE_DATA_SIZE   (27)
#endif

/* structure to hold registered timers */
typedef struct
{
    TIMER_LIST_ENT          *p_tle;      /* timer entry */
    tBTU_TIMER_CALLBACK     timer_cb;    /* callback triggered when timer expires */
} tBTU_TIMER_REG;

/* structure to hold registered event callbacks */
typedef struct
{
    UINT16                  event_range;  /* start of event range */
    tBTU_EVENT_CALLBACK     event_cb;     /* callback triggered when event is in range */
} tBTU_EVENT_REG;

#if (defined(HCILP_INCLUDED) && HCILP_INCLUDED == TRUE)
#define HCILP_STATE_ACTIVE      0
#define HCILP_STATE_W4_TX_DONE  1
#define HCILP_STATE_W4_TIMEOUT  2
#define HCILP_STATE_LOW_POWER   3
/* HCILP Control Block */
typedef struct
{
    BOOLEAN         lp_enabled;
    BOOLEAN         sent_vsc_lp_enabled;    /* whether we send vsc lp enable to controller */
    BOOLEAN         lp_no_tx_data;
    UINT8           state;
    TIMER_LIST_ENT  lp_timer_list;
    tHCILP_ENABLE_CBACK    *p_enable_cback;
    tHCILP_WAKEUP_BT_CBACK *p_wakeup_cback;
    tHCILP_Params   params;
#if (defined(H4IBSS_INCLUDED) && H4IBSS_INCLUDED == TRUE)
    tH4IBSS_CB      h4ibss_cb;
#endif
} tHCILP_CB;

#endif

/* HCIUTIL control block */
typedef struct
{
    UINT8                         vs_baudrate;
    tHCIUTIL_UPDATEBAUDRATE_CBACK *p_cback;
} tHCIUTIL_CB;

/* 1 NFC transport for test chip; 0 for combo chip */
#if (NFC_INCLUDED == TRUE)
#define NFC_MAX_LOCAL_CTRLS     1
#else
#define NFC_MAX_LOCAL_CTRLS     0
#endif

/* the index to BTU command queue array */
#define NFC_CONTROLLER_ID       (1)

#define BTU_MAX_LOCAL_CTRLS     (1 + NFC_MAX_LOCAL_CTRLS) /* only BR/EDR */

/* HCI control block */
typedef struct
{
    BUFFER_Q         cmd_xmit_q;
    BUFFER_Q         cmd_cmpl_q;
    UINT16           cmd_window;
    TIMER_LIST_ENT   cmd_cmpl_timer;        /* Command complete timer */
#if (defined(BTU_CMD_CMPL_TOUT_DOUBLE_CHECK) && BTU_CMD_CMPL_TOUT_DOUBLE_CHECK == TRUE)
    BOOLEAN          checked_hcisu;
#endif
#if (defined BTU_HANDLE_RAW_HCI_ENABLED) && (BTU_HANDLE_RAW_HCI_ENABLED == TRUE) /* BSA_SPECIFIC */
    UINT16           test_mode_cmd;
#endif
} tHCI_CMD_CB;

/* The events reported on tBTM_VS_EVT_HDLR */
enum
{
    BTU_VS_RESET_CONT_EVT =        1,
    BTU_VS_FEAT_INIT_EVT,
    BTU_VS_CFG_CODEC_EVT,
    BTU_VS_SET_WBS_CODEC_EVT,
    BTU_VS_CFG_SCO_PATH_EVT,
    BTU_VS_INT_TIMEOUT_EVT,
    BTU_VS_H4IBSS_EVT,
    BTU_VS_L2C_CTRL_ENC_EVT,
    BTU_VS_TBFC_CONN_EVT,
    BTU_VS_TBFC_RESET_EVT,
    BTU_VS_TBFC_SUSPEND_EVT,
    BTU_VS_TBFC_LE_NOTIFY_EVT,
    BTU_VS_READ_BPCS_EVT,
    BTU_VS_TBFC_TRIGGER_EVT,
    BTU_VS_LE_BURST_DATA_EVT,
    BTU_VS_SET_LOCAL_ADDR_EVT,
    BTU_VS_TOPOLOGY_CHECK_EVT,
    BTU_VS_MULTI_ADV_PRIVACY_EVT,
    BTU_VS_MULTI_ADV_REFRESH_RPA_EVT,
    BTU_VS_MULTI_ADV_REENABLE_EVT
};
typedef UINT16   tBTU_INT_EVT;
/* vendor specific function callback */
typedef UINT8 (tBTU_VS_EVT_HDLR) (tBTU_INT_EVT event, void *p);

/* Define structure holding BTU variables
*/
typedef struct
{
    tBTU_TIMER_REG   timer_reg[BTU_MAX_REG_TIMER];
    tBTU_EVENT_REG   event_reg[BTU_MAX_REG_EVENT];

    TIMER_LIST_Q  quick_timer_queue;        /* Timer queue for transport level (100/10 msec)*/
    TIMER_LIST_Q  timer_queue;              /* Timer queue for normal BTU task (1 second)   */
#if HCITHIN_INCLUDED == TRUE
    TIMER_LIST_ENT   cmd_cmpl_timer;        /* Command complete timer */
#endif
    UINT16    hcit_acl_data_size;           /* Max ACL data size across HCI transport    */
    UINT16    hcit_acl_pkt_size;            /* Max ACL packet size across HCI transport  */
                                            /* (this is data size plus 4 bytes overhead) */

#if BLE_INCLUDED == TRUE
    UINT16    hcit_ble_acl_data_size;           /* Max BLE ACL data size across HCI transport    */
    UINT16    hcit_ble_acl_pkt_size;            /* Max BLE ACL packet size across HCI transport  */
                                            /* (this is data size plus 4 bytes overhead) */
#endif

    BOOLEAN     reset_complete;             /* TRUE after first ack from device received */
    UINT8       trace_level;                /* Trace level for HCI layer */

#if (defined(HCILP_INCLUDED) && HCILP_INCLUDED == TRUE)
    tHCILP_CB   hcilp_cb;
#endif
    tHCIUTIL_CB hciutil_cb;

#if (BTU_DUAL_STACK_MM_INCLUDED == TRUE) || (BTU_DUAL_STACK_BTC_INCLUDED == TRUE) || (BTU_STACK_LITE_ENABLED == TRUE)
#define BTU_FULL_STACK_ACTIVE       0
#define BTU_LITE_STACK_ACTIVE       1
#define BTU_TRANSPORT_HOLD          2
#define BTU_FULL_TRANSPORT_ACTIVE   3
#define BTU_LITE_TRANSPORT_ACTIVE   4
    UINT8     transport_state;
#endif

#if (BTU_DUAL_STACK_MM_INCLUDED == TRUE) || (BTU_DUAL_STACK_BTC_INCLUDED == TRUE)
    BOOLEAN     hci_over_ipc;
#endif

    tHCI_CMD_CB hci_cmd_cb[BTU_MAX_LOCAL_CTRLS]; /* including BR/EDR */

    /* the data members in this section may be changed by BTU_<VendorName>Init() */
    tBTU_VS_EVT_HDLR       *p_vs_evt_hdlr;             /* VS processing for internal events */
} tBTU_CB;

#if (BTU_DUAL_STACK_MM_INCLUDED == TRUE) || (BTU_DUAL_STACK_BTC_INCLUDED == TRUE) || (BTU_STACK_LITE_ENABLED == TRUE)
#define BTU_IPC_CMD_SET_TRANSPORT_STATE 0   /* Set transport state (param=transprt state) */
#define BTU_IPC_CMD_DISABLE_TRANSPORT   1   /* Set transport hardware (param=1 to disable) */
typedef struct
{
    UINT8   op_code;
    UINT8   param;
} tBTU_IPC_CMD;

#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Global BTU data */
#if BTU_DYNAMIC_MEMORY == FALSE
BTU_API extern tBTU_CB  btu_cb;
#else
BTU_API extern tBTU_CB *btu_cb_ptr;
#define btu_cb (*btu_cb_ptr)
#endif

BTU_API extern const BD_ADDR        BT_BD_ANY;

/* Functions provided by btu_task.c
************************************
*/
BTU_API extern void btu_start_timer (TIMER_LIST_ENT *p_tle, UINT16 type, UINT32 timeout);
BTU_API extern void btu_stop_timer (TIMER_LIST_ENT *p_tle);
BTU_API extern void btu_register_timer (TIMER_LIST_ENT *p_tle, UINT16 type, UINT32 timeout, tBTU_TIMER_CALLBACK timer_cb);
BTU_API extern void btu_deregister_timer(TIMER_LIST_ENT *p_tle);
BTU_API extern UINT32 btu_remaining_time (TIMER_LIST_ENT *p_tle);


BTU_API extern void btu_register_event_range (UINT16 range, tBTU_EVENT_CALLBACK event_cb);
BTU_API extern void btu_deregister_event_range (UINT16 range);
BTU_API extern void btu_uipc_rx_cback(BT_HDR *p_msg);

BTU_API extern void btu_hcif_flush_cmd_queue(void);
/*
** Quick Timer
*/
#if defined(QUICK_TIMER_TICKS_PER_SEC) && (QUICK_TIMER_TICKS_PER_SEC > 0)
#define QUICK_TIMER_TICKS (GKI_SECS_TO_TICKS (1)/QUICK_TIMER_TICKS_PER_SEC)
BTU_API extern void btu_start_quick_timer (TIMER_LIST_ENT *p_tle, UINT16 type, UINT32 timeout);
BTU_API extern void btu_stop_quick_timer (TIMER_LIST_ENT *p_tle);
BTU_API extern void btu_process_quick_timer_evt (void);
BTU_API extern void process_quick_timer_evt (TIMER_LIST_Q *p_tlq);
#endif

#if (defined(HCILP_INCLUDED) && HCILP_INCLUDED == TRUE)
BTU_API extern void btu_check_bt_sleep (void);
#endif

/* Functions provided by btu_hcif.c
************************************
*/
BTU_API extern void  btu_hcif_process_event (UINT8 controller_id, BT_HDR *p_buf);
BTU_API extern void  btu_hcif_send_cmd (UINT8 controller_id, BT_HDR *p_msg);
BTU_API extern void  btu_hcif_send_host_rdy_for_data(void);
BTU_API extern void  btu_hcif_cmd_timeout (UINT8 controller_id);

/* Functions provided by btu_core.c
************************************
*/
BTU_API extern void  btu_init_core(void);
BTU_API extern void  BTE_Init(void);
BTU_API extern UINT16 BTU_AclPktSize(void);
BTU_API extern UINT16 BTU_BleAclPktSize(void);

#if (BTU_DUAL_STACK_MM_INCLUDED == TRUE) || (BTU_DUAL_STACK_BTC_INCLUDED == TRUE) || (BTU_STACK_LITE_ENABLED == TRUE)
BTU_API extern void BTU_SetTransportState(UINT8 state);
BTU_API extern UINT8 BTU_ReadTransportState(void);
#endif

#if (BTU_DUAL_STACK_MM_INCLUDED == TRUE) || (BTU_DUAL_STACK_BTC_INCLUDED == TRUE)
BTU_API extern void BTU_SetLiteTransportState(UINT8 state);
BTU_API extern void BTU_DisableLiteHciTransport(BOOLEAN bDisable);

/* Functions provided by IPC
************************************
*/
BTU_API extern BOOLEAN BTU_IsHciOverIpc(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
