#ifndef IAT_SAMPLE_H_
#define IAT_SAMPLE_H_

enum IatState
{
    IAT_STATE_NONE,
    IAT_STATE_ERROR,
    IAT_STATE_VAD,
    IAT_STATE_CONTINUE,
    IAT_STATE_COMPLETE,
};

#define g_buffersize 4096

class IatProcess
{
  public:
    static const char* beginQISRSession(const char* params);
    static IatState writeISRAudioData(const char* sessionId, const char* audioData,
                                      unsigned int audioLen, bool newSession);

    static char iatResult[g_buffersize];
    static void endQISRSession(const char* sessionId);
};

#endif /* IAT_SAMPLE_H_ */
