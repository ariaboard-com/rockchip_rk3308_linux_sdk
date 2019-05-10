////
// Created by eddy on 18-1-30.
//

#ifndef DUEROS_DCS_APP_DLNA_DLNADMRLOGGING_H
#define DUEROS_DCS_APP_DLNA_DLNADMRLOGGING_H

#include "LoggerUtils/DcsSdkLogger.h"

#define dbg(fmt, ...) APP_DEBUG("[dlna] " fmt, ##__VA_ARGS__)
#define err(fmt, ...) APP_ERROR("[dlna] " fmt, ##__VA_ARGS__)

#endif //DLNA_DMR_LOGGING_H
