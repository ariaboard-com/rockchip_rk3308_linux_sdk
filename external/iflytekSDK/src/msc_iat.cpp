#include "msc_iat.h"
#include "log.h"
#include "qisr.h"
#include "msp_errors.h"

#include <unistd.h>
#include <string.h>

char IatProcess::iatResult[g_buffersize] = "";

/**
 * @brief IatProcess::beginQISRSession
 * @param params
 * @return sessionId Return NULL if ISRSession begin failed
 */
const char* IatProcess::beginQISRSession(const char *params)
{
    const char* sessionId = NULL;
    int	ret = MSP_SUCCESS;

    sessionId = QISRSessionBegin(NULL, params, &ret);
    if(ret != MSP_SUCCESS)
    {
        Log::error("QISRSessionBegin failed! error code:%d\n", ret);
        goto iat_exit;
    }

    return sessionId;

iat_exit:
    QISRSessionEnd(sessionId, NULL);
    return sessionId;
}

void IatProcess::endQISRSession(const char* sessionId)
{
    if(sessionId != NULL)
    {
        QISRSessionEnd(sessionId, NULL);
    }
}

/**
 * @brief IatProcess::writeISRAudioData
 *
 * @param sessionId
 * @param audioData
 * @param audioLen
 * @param newSession
 * @return IatState
 */
IatState IatProcess::writeISRAudioData(const char *sessionId, const char *audioData,
                                       unsigned int audioLen, bool newSession)
{
    int ret = 0;
    int	aud_stat = MSP_AUDIO_SAMPLE_CONTINUE;
    int	ep_stat	= MSP_EP_LOOKING_FOR_SPEECH;
    int	rec_stat = MSP_REC_STATUS_SUCCESS;
    IatState iatState = IAT_STATE_NONE;

    if(newSession)
    {
        Log::info("Start listening.\n");
        aud_stat = MSP_AUDIO_SAMPLE_FIRST;
    }

    ret = QISRAudioWrite(sessionId, audioData, audioLen, aud_stat, &ep_stat, &rec_stat);
    if(ret != MSP_SUCCESS)
    {
        Log::error("QISRAudioWrite failed errCode: %d\n", ret);
        iatState = IAT_STATE_ERROR;
        goto iat_exit;
    }

    if(ep_stat == MSP_EP_AFTER_SPEECH)
    {
        ret = QISRAudioWrite(sessionId, NULL, 0, MSP_AUDIO_SAMPLE_LAST, &ep_stat, &rec_stat);
        if(ret != MSP_SUCCESS)
        {
            Log::error("QISRAudioWrite(MSP_AUDIO_SAMPLE_LAST) failed errCode: %d\n", ret);
            iatState = IAT_STATE_ERROR;
        }

        iatState = IAT_STATE_VAD;
        memset(iatResult, 0, g_buffersize);
        while(rec_stat != MSP_REC_STATUS_COMPLETE)
        {
            const char *rslt = QISRGetResult(sessionId, &rec_stat, 0, &ret);
            if(ret != MSP_SUCCESS)
            {
                Log::error("QISRGetResult failed with errCode-: %d\n", ret);
                iatState = IAT_STATE_ERROR;
                goto iat_exit;
            }

            if(rslt != NULL)
            {
                unsigned int rslt_len = strlen(rslt);
                strncat(iatResult, rslt, rslt_len);
                iatState = IAT_STATE_COMPLETE;
            }

            usleep(150 * 1000); // Prevent frequent CPU usage
        }

        goto iat_exit;
    }
    else
    {
        iatState = IAT_STATE_CONTINUE;
    }

    return iatState;

iat_exit:
    QISRSessionEnd(sessionId, NULL);
    return iatState;
}
