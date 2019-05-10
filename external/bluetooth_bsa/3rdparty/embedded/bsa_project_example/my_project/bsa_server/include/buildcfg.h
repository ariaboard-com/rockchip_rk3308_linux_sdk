#ifndef BUILDCFG_H
#define BUILDCFG_H
#define HCITHIN_INCLUDED   FALSE
#define HCILP_INCLUDED   FALSE
#define HCISU_H4_INCLUDED   TRUE
#define BTM_SCO_HCI_INCLUDED   FALSE
#define BSA_SERIAL_DEV_NAME   "/dev/btusb0"
#define BTSNOOPDISP_INCLUDED   TRUE
#define BT_BRCM_VS_INCLUDED   TRUE
#define GAP_INCLUDED   TRUE
#define BTA_GATT_INCLUDED   TRUE
#define BLE_INCLUDED   TRUE
#define ATT_INCLUDED   TRUE
#define GATT_SERVER_ENABLED   TRUE
#define GATT_CLIENT_ENABLED   TRUE
#define BTA_GATTC_CL_MAX   4
#define BSA_BLE_CLIENT_MAX   (BTA_GATTC_CL_MAX - 1)
#define SMP_INCLUDED   TRUE
#define BTA_GATT_DEBUG   TRUE
#define BTA_HH_LE_INCLUDED   TRUE
#define BTA_GATTC_NOTIF_REG_MAX   10
#define BLE_AUDIO_INCLUDED   TRUE
#define SBC_FOR_EMBEDDED_LINUX   TRUE
#define L2CAP_INCLUDED   TRUE
#define L2CAP_COMPRESSION_INCLUDED   FALSE
#define L2CAP_HOST_FLOW_CTRL         FALSE
#define L2CAP_FCR_INCLUDED   TRUE
#define L2CAP_HCI_FLOW_CONTROL_DEBUG   FALSE
#define MAX_L2CAP_LINKS   7
#define MAX_L2CAP_CHANNELS   20
#define MAX_L2CAP_CLIENTS   16
#define L2CAP_DESIRED_LINK_ROLE   HCI_ROLE_MASTER
#define L2CAP_EXTFEA_SUPPORTED_MASK   (L2CAP_EXTFEA_ENH_RETRANS | L2CAP_EXTFEA_STREAM_MODE | L2CAP_EXTFEA_NO_CRC | L2CAP_EXTFEA_UCD_RECEPTION)
#define L2CAP_UCD_INCLUDED   TRUE
#define SDP_DI_INCLUDED   TRUE
#define SDP_RAW_DATA_INCLUDED   TRUE
#define SDP_MAX_PAD_LEN   1024
#define SDP_MTU_SIZE   510
#define RFCOMM_INCLUDED   TRUE
#define MAX_BD_CONNECTIONS   4
#define OBX_INCLUDED   FALSE
#define OBX_SERVER_INCLUDED   FALSE
#define OBX_CLIENT_INCLUDED   FALSE
#define OBX_MD5_INCLUDED      FALSE
#define GOEP_INCLUDED   FALSE
#define GOEP_FS_INCLUDED   FALSE
#define BTA_DG_INCLUDED   TRUE
#define SPP_INCLUDED   FALSE
#define DUN_INCLUDED   TRUE
#define BTA_AG_INCLUDED   FALSE
#define BTA_HS_INCLUDED   FALSE
#define HSP2_INCLUDED   FALSE
#define HFP_INCLUDED   FALSE
#define OPP_INCLUDED   FALSE
#define OPP_SERVER_INCLUDED   FALSE
#define OPP_CLIENT_INCLUDED   FALSE
#define BTA_OP_INCLUDED   FALSE
#define BTA_OPS_INCLUDED   FALSE
#define BTA_OPC_INCLUDED   FALSE
#define BTA_OPS_DEBUG   FALSE
#define FTP_INCLUDED   FALSE
#define FTP_SERVER_INCLUDED   FALSE
#define FTP_CLIENT_INCLUDED   FALSE
#define BTA_FT_INCLUDED   FALSE
#define BTA_FTS_INCLUDED   FALSE
#define BTA_FTC_INCLUDED   FALSE
#define BIP_INCLUDED   FALSE
#define BIP_INITR_INCLUDED   FALSE
#define BIP_RSPDR_INCLUDED   FALSE
#define BTA_BI_INCLUDED   FALSE
#define BIP_PRINTING_INCLUDED   FALSE
#define BIP_PUSH_INCLUDED   FALSE
#define BIP_PULL_INCLUDED   FALSE
#define BIP_ARCHIVE_INCLUDED   FALSE
#define BIP_CAMERA_INCLUDED   FALSE
#define BIP_DISPLAY_INCLUDED   FALSE
#define BPP_INCLUDED   FALSE
#define BPP_SND_INCLUDED   FALSE
#define HCRP_INCLUDED   FALSE
#define HCRP_CLIENT_INCLUDED   FALSE
#define HCRP_SERVER_INCLUDED   FALSE
#define BTA_PR_INCLUDED   FALSE
#define BTA_RC_INCLUDED   TRUE
#define AVCT_INCLUDED   TRUE
#define AVRC_INCLUDED   TRUE
#define AVDT_INCLUDED   TRUE
#define A2D_INCLUDED   TRUE
#define A2D_SBC_INCLUDED   TRUE
#define A2D_M12_INCLUDED   TRUE
#define A2D_M24_INCLUDED   TRUE
#define VDP_INCLUDED       TRUE
#define VDP_H263_INCLUDED   TRUE
#define VDP_MPEG_INCLUDED   TRUE
#define VDP_VEND_INCLUDED   TRUE
#define BTA_AV_INCLUDED   TRUE
#define BTA_AVK_INCLUDED   TRUE
#define BTA_AR_INCLUDED   TRUE
#define BSA_AV_UIPC_BUFFER_SIZE   20001
#define BSA_AV_SBC_CAPS_SAMP_FREQ   A2D_SBC_IE_SAMP_FREQ_48
#define BSA_AV_SBC_DEFAULT_SAMP_FREQ   A2D_SBC_IE_SAMP_FREQ_48
#define BSA_AV_SBC_CH_MODE   A2D_SBC_IE_CH_MD_JOINT
#define BSA_AV_SBC_BLOCK_LEN   A2D_SBC_IE_BLOCKS_16
#define BSA_AV_SBC_NUM_SUBBAND   A2D_SBC_IE_SUBBAND_8
#define BSA_AV_SBC_ALLOC_MD   A2D_SBC_IE_ALLOC_MD_L
#define BSA_AV_SBC_MAX_BP   0x59
#define BSA_AV_SBC_MIN_BP   0x5
#define BSA_MEDIA_AV_FLOW_DEBUG   FALSE
#define BSA_MEDIA_BITRATE_STEP   10
#define BSA_MEDIA_OVERFEED_INCLUDED   FALSE
#define BSA_MEDIA_UNDERFEED_INCLUDED   FALSE
#define AVDT_DEBUG   TRUE
#define BSA_AVK_SEC_SUPPORT   FALSE
#define BSA_SV_AV_AA_SBC_INDEX    0  
#define BSA_SV_AV_AA_SEC_INDEX    1
#define BSA_SV_AV_AA_APTX_INDEX   2
#define BSA_SV_AV_AA_SEP_MAX    3
#define AVDT_NUM_SEPS   6
#define BSA_AV_SBC_RATE_LEVEL_0   250
#define BSA_AV_SBC_RATE_LEVEL_1   250
#define BSA_AV_SBC_RATE_LEVEL_2   200
#define BSA_AV_SBC_RATE_LEVEL_3   150
#define BSA_AV_SBC_RATE_LEVEL_4   120
#define BSA_AV_SBC_RATE_LEVEL_5   100
#define BSA_AV_SEC_RATE_LEVEL_0   348
#define BSA_AV_SEC_RATE_LEVEL_1   348
#define BSA_AV_SEC_RATE_LEVEL_2   348
#define BSA_AV_SEC_RATE_LEVEL_3   348
#define BSA_AV_SEC_RATE_LEVEL_4   348
#define BSA_AV_SEC_RATE_LEVEL_5   348
#define BSA_AV_BITRATE_CONTROL_BY_USER   FALSE
#define BTA_BAV_INCLUDED   FALSE
#define BTA_BAV_DEBUG   TRUE
#define BTA_BAV_STREAM_MAX   2
#define BTA_AV_RC_COMP_ID   AVRC_CO_METADATA
#define BTA_AVK_RC_COMP_ID   AVRC_CO_METADATA
#define SBC_FOR_EMBEDDED_LINUX   TRUE
#define BTA_AC_INCLUDED   FALSE
#define BTA_CT_INCLUDED   FALSE
#define BTA_CG_INCLUDED   FALSE
#define CTP_INCLUDED   FALSE
#define TCS_INCLUDED   FALSE
#define ICP_INCLUDED   FALSE
#define ICA_INCLUDED   FALSE
#define BNEP_INCLUDED   FALSE
#define PAN_INCLUDED   FALSE
#define BTA_PAN_INCLUDED   FALSE
#define HID_HOST_INCLUDED   TRUE
#define BTA_HH_DEBUG   TRUE
#define BTA_HH_INCLUDED   TRUE
#define HID_HOST_MAX_CONN_RETRY   1
#define BTA_HH_ROLE   BTA_MASTER_ROLE_ONLY
#define BTM_SEC_MAX_DEVICE_RECORDS   40
#define HID_HOST_MAX_DEVICES   20
#define HID_DEV_INCLUDED   TRUE
#define BTA_HD_INCLUDED   TRUE
#define HID_DEV_MTU_SIZE   200
#define HID_HOST_DEV_COEX_SUPPORTED   TRUE
#define HID_DEV_SET_CONN_MODE   FALSE
#define BTA_HD_CUST_REPT_SIZE   100
#define SAP_SERVER_INCLUDED   FALSE
#define SAP_INCLUDED   FALSE
#define BTA_PBS_INCLUDED   FALSE
#define BTA_PBC_INCLUDED   FALSE
#define BTA_SS_INCLUDED   FALSE
#define BTA_SC_INCLUDED   FALSE
#define BTA_FM_INCLUDED   FALSE
#define BTA_FMTX_INCLUDED   FALSE
#define BTA_HL_INCLUDED   TRUE
#define BTA_HL_DEBUG   TRUE
#define BTA_HL_DISC_SIZE   3000
#define BTA_MIP_INCLUDED   FALSE
#define SLIP_INCLUDED   FALSE
#define SLIP_STATIS_INCLUDED   FALSE
#define SLIP_SW_FLOW_CTRL   FALSE
#define BT_TRACE_SLIP   FALSE
#define SLIP_SLEEP_TO   5000
#define SLIP_HOST_SLIDING_WINDOW_SIZE   7
#define MCA_INCLUDED   TRUE
#define MCA_SYNC_INCLUDED   TRUE
#define MCA_NUM_DEPS   8
#define MCA_NUM_MDLS   8
#define BTA_HL_NUM_MDEPS   8
#define BTA_BRCM_INCLUDED   TRUE
#define BTA_DM_INCLUDED   TRUE
#define BTA_FS_INCLUDED   TRUE
#define BTA_DI_INCLUDED   TRUE
#define BTA_EIR_CANNED_UUID_LIST   FALSE
#define GPS_INCLUDED   FALSE
#define BTA_EIR_ADDITIONAL_DATA_FIRST   TRUE
#define BTA_SYS_TIMER_PERIOD   100
#define BTA_DM_REMOTE_DEVICE_NAME_LENGTH   248
#define BSA_POWER_INCLUDED   TRUE
#define BSA_POWER_INQ_TX_RSP_INCLUDED   TRUE
#define BTA_AV_ROLE   BTA_MASTER_ROLE_ONLY
#define BTA_DM_SCATTERNET   BTA_DM_NO_SCATTERNET
#define BTA_DM_PAGE_TIMEOUT   8192
#define BTM_INQ_DB_SIZE   50
#define BTM_INQ_GET_REMOTE_NAME   FALSE
#define BTM_SCO_INCLUDED   FALSE
#define BTM_LISBON_INCLUDED   TRUE
#define BTM_OOB_INCLUDED   FALSE
#define BTM_MAX_REM_BD_NAME_LEN   100
#define BTM_DEFAULT_CONN_INTERVAL   800
#define PATCHRAM_INCLUDED   TRUE
#define BTM_APP_DEV_INIT   bta_sys_dev_init
#define BTM_SSR_INCLUDED   FALSE
#define BTM_ALLOW_CONN_IF_NONDISCOVER   TRUE
#define BTU_BTA_INCLUDED   TRUE
#define BTU_STACK_LITE_ENABLED   FALSE
#define BTTRC_INCLUDED   FALSE
#define BTTRC_PARSER_INCLUDED   FALSE
#define MAX_TRACE_RAM_SIZE   10000
#define TRACE_TASK_INCLUDED   TRUE
#define BT_TRACE_PROTOCOL   TRUE
#define BT_USE_TRACES   TRUE
#define BT_TRACE_APPL   TRUE
#define BT_USE_TRACES_TIMESTAMP   TRUE
#define BSA_COLORED_TRACES   TRUE
#define BSA_INITIAL_TRACE_LEVEL   BT_TRACE_LEVEL_DEBUG
#define BTM_INITIAL_TRACE_LEVEL   BT_TRACE_LEVEL_DEBUG
#define L2CAP_INITIAL_TRACE_LEVEL   BT_TRACE_LEVEL_DEBUG
#define APPL_INITIAL_TRACE_LEVEL   BT_TRACE_LEVEL_DEBUG
#define AVDT_INITIAL_TRACE_LEVEL   BT_TRACE_LEVEL_DEBUG
#define GKI_NUM_FIXED_BUF_POOLS   9
#define GKI_BUF3_SIZE   4096
#define GKI_BUF3_MAX   100
#define BRCM_GKI_INCLUDED   TRUE
#define GKI_USE_PTHREADS   TRUE
#define GKI_TIMER_LIST_NOPREEMPT   TRUE
#define UCODEC_TASK   3
#define BT_MEDIA_TASK   UCODEC_TASK
#define BSA_TM_INCLUDED   TRUE
#define BSA_NSA_INCLUDED   FALSE
#define BTAPP_INCLUDED   TRUE
#define BTA_CONFIG_PATH_LEN   500
#define BTA_CONFIG_FILE_LEN   300
#define BTA_DEVICES_PATH_LEN   500
#define BTA_DEVICES_FILE_LEN   300
#define BTA_FS_PATH_LEN   500
#define BTA_FS_FILE_LEN   300
#define BTA_DM_CO_INCLUDE_BTUI   FALSE
#define LINUX_OS   TRUE
#define GKI_SIGNAL_HANDLER_EXCLUDED   TRUE
#define BTM_SEC_TIMEOUT_VALUE   30
#define BTA_DM_COD   {0x00, 0x04, 0x3C}
#define BTA_DM_LINK_TIMEOUT   4800
#define AT91_MAIN_INCLUDED   FALSE  
#define AT91_DRV_INCLUDED   FALSE
#define AT91_LIB_INCLUDED   FALSE
#define AT91_GKI_INCLUDED   FALSE
#define UNV_INCLUDED   FALSE
#define BBY_MAIN_INCLUDED   FALSE
#define BRCM_LIB_INCLUDED   TRUE
#define BRCM_DRV_INCLUDED   TRUE
#define MMI_INCLUDED   FALSE
#define BTA_JV_INCLUDED   FALSE
#define RPC_INCLUDED   FALSE
#define RSI_INCLUDED   TRUE
#define RPC_TRACE_ONLY   FALSE
#define BSA_SET_THREAD_NAME   FALSE
#endif
