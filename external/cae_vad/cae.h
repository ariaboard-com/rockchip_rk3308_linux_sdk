/*
 * cae.h
 *
 *  Created on: 2015年8月24日
 *      Author: admin
 */

#ifndef JNI_SRC_CAE_H_
#define JNI_SRC_CAE_H_

typedef void * CAE_HANDLE;

typedef void (*cae_ivw_fn)(short angle, short channel, float power, short CMScore, short beam, char *param1, void *param2, void *userData);

typedef void (*cae_audio_fn)(const void *audioData, unsigned int audioLen, int param1, const void *param2, void *userData);

#ifdef __cplusplus
extern "C" {
#endif

int CAENew(CAE_HANDLE *cae, const char* resPath, cae_ivw_fn ivwCb, cae_audio_fn audioCb, const char*param, void *userData);

int CAEAudioWrite(CAE_HANDLE cae, const void *audioData, unsigned int audioLen);

int CAEResetEng(CAE_HANDLE cae);

int CAEDestroy(CAE_HANDLE cae);

int CAESetRealBeam(CAE_HANDLE cae, int beam);

int CAEExtract16K(void *in, int in_framelen, int channel, void *out, int *out_framelen);

#ifdef __cplusplus
}
#endif

#endif /* JNI_SRC_CAE_H_ */
