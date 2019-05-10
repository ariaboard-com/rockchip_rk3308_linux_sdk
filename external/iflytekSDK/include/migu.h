#ifndef __MIGU_H
#define __MIGU_H


#include <curl/curl.h>
#include <cjson/cJSON.h>

#ifdef __cplusplus
extern "C" {
#endif


char * getSdkVersion(char * version);
int migusdk_init( char * device_info);
char * get_musicInfo(char * musicId, char *result);
char * get_musicInfo_key(char * keybuf, char *result);
char * get_musicInfo_musicSheetId(char * musicSheetId, char *result);


#ifdef __cplusplus
} /* extern "C" */
#endif /* C++ */

#endif
