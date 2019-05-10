/*****************************************************************************
 **
 **  Name:           app_hh_opus.h
 **
 **  Description:    APP HID OPUS function definitions
 **
 **  Copyright (c) 2017, Cypress Semiconductor, All Rights Reserved.
 **  Proprietary and confidential.
 **
 *****************************************************************************/
#ifndef _APP_HH_OPUS_H_
#define _APP_HH_OPUS_H_

/*******************************************************************************
 **
 ** Function         app_hh_opus_init
 **
 ** Description      This function initialize OPUS decoder
 **
 ** Parameters
 **
 ** Returns          0 if successful, otherwise the error number
 **
 *******************************************************************************/
int app_hh_opus_init(void);

/*******************************************************************************
 **
 ** Function         app_hh_opus_decode
 **
 ** Description      This function processes HID reports containing OPUS encoded
 **                  data.
 **
 ** Parameters
 **
 ** Returns          void
 **
 *******************************************************************************/
void app_hh_opus_decode(tAPP_HH_DEVICE *p_hh_dev, UINT8 *p_data, UINT16 length);

#endif /* _APP_HH_OPUS_H_ */
