#ifndef AUDIO_PROCESS_H_
#define AUDIO_PROCESS_H_

#include "cae_lib.h"

class AudioProcess
{
public:
    static const char* getIatSessionId();
    static bool MSP_Login(const char* loginParams);
    static bool paddingIatAudio(const char* audioData, unsigned int audioLen,
                                CAEUserData *userData);

private:
    static const char* iatSessionId;
};

#endif // AUDIO_PROCESS_H_
