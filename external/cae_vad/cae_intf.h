/**
 * @file    cae_intf.h
 * @brief   CAE Interface Header File
 * 
 * @version 1.0
 * @date    2015/08/25
 * 
 * @see        
 * 
 * History:
 * index    version        date            author        notes
 * 0        1.0            2015/08/25      kunzhang      Create this file
 * 0        2.0            2016/01/09      cyhu          Modiy  this file
 */
 
#ifndef __CAE_INTF_H__
#define __CAE_INTF_H__

typedef void * CAE_HANDLE; 

typedef void (*cae_ivw_fn)(short angle, short channel, float power, short CMScore, short beam, char *param1, void *param2, void *userData);

typedef void (*cae_audio_fn)(const void *audioData, unsigned int audioLen, int param1, const void *param2, void *userData);

typedef void (*cae_ivw_audio_fn)(const void *audioData, unsigned int audioLen, int param1, const void *param2, void *userData);

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int CAENew(CAE_HANDLE *cae, const char* resPath, cae_ivw_fn ivwCb, cae_ivw_audio_fn ivwAudioCb, cae_audio_fn audioCb, const char *param, void *userData);
typedef int (* Proc_CAENew)(CAE_HANDLE *cae, const char* resPath, cae_ivw_fn ivwCb, cae_ivw_audio_fn ivwAudioCb, cae_audio_fn audioCb, const char *param, void *userData);

int CAEReloadResource(CAE_HANDLE cae, const char* resPath);
typedef int (* Proc_CAEReloadResource)(CAE_HANDLE cae, const char* resPath);

int CAEAudioWrite(CAE_HANDLE cae, const void *audioData, unsigned int audioLen);
typedef int (* Proc_CAEAudioWrite)(CAE_HANDLE cae, const void *audioData, unsigned int audioLen);

int CAEResetEng(CAE_HANDLE cae);
typedef int (* Proc_CAEResetEng)(CAE_HANDLE cae);

int CAEGetAGCValue(CAE_HANDLE cae);
typedef int (* Proc_CAEGetAGCValue)(CAE_HANDLE cae);

int CAESetRealBeam(CAE_HANDLE cae, int beam);
typedef int (* Proc_CAESetRealBeam)(CAE_HANDLE cae, int beam);

int CAESetListener(CAE_HANDLE cae, const char* param, void* listener);
typedef int (* Proc_CAESetListener)(CAE_HANDLE cae, const char* param, void* listener);

int CAESetWParam(CAE_HANDLE cae, const char* param, const char* value);
typedef int (* Proc_CAESetWParam)(CAE_HANDLE cae, const char* param, const char* value);

int CAEGetWParam(CAE_HANDLE cae, const char* param, char* value, unsigned int *valueLen);
typedef int (* Proc_CAEGetWParam)(CAE_HANDLE cae, const char* param, char* value, unsigned int *valueLen);

char* CAEGetVersion();
typedef char (* Proc_CAEGetVersion)();

int CAEDestroy(CAE_HANDLE cae);
typedef int (* Proc_CAEDestroy)(CAE_HANDLE cae);

int CAEGetChannel();
typedef int (* Proc_CAEGetChannel)();

int CAESetShowLog(int show_log);
typedef int (* Proc_CAESetShowLog)(int show_log);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CAE_INTF_H__ */