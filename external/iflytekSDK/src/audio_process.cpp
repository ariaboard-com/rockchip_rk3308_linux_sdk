#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <thread>

#include "audio_process.h"
#include "log.h"
#include "msc_iat.h"
#include "msc_tts.h"
#include "constant.h"
#include "qisr.h"
#include "msp_cmn.h"
#include "msp_errors.h"
#include "cJSON.h"
#include "gst_player.h"
#include "cmcc.h"
#include "device_info.h"

const char* AudioProcess::iatSessionId = NULL;
const char* parseJsonStr(const char* JsonStr)
{
    const char *sAnswer;
    const char *service;

    cJSON *pJson = cJSON_Parse(JsonStr);
    if(pJson == NULL)
    {
        Log::error("cJSON_Parse failed\n");
        return NULL;
    }
    cJSON *pSetviceType = cJSON_GetObjectItem(pJson, "service");
    if(pSetviceType != NULL)
    {
        service = pSetviceType->valuestring;
        Log::info("----parse_json_result,servie =%s---\n", service);
        if(!strcmp(service, "weather"))
        {
            cJSON *pAnswer = cJSON_GetObjectItem(pJson, "data");
            if(NULL == pAnswer)
            {
                Log::info("---no data--\n");
                return Constant::kJsonQuestionNotFound;;
            }
            cJSON *pResult = cJSON_GetObjectItem(pAnswer, "result");
            if(NULL == pResult)
            {
                Log::info("----no result---\n");
                return Constant::kJsonQuestionNotFound;;
            }
            cJSON *pArray = cJSON_GetArrayItem(pResult, 0);
            if(NULL == pArray)
            {
                Log::info("----no array---\n");
                return Constant::kJsonQuestionNotFound;;
            }

            cJSON *pWeather = cJSON_GetObjectItem(pArray, "weather");
            cJSON *pTemp = cJSON_GetObjectItem(pArray, "tempRange");
            cJSON *pWind = cJSON_GetObjectItem(pArray, "wind");

			bzero(feedback_answer, sizeof(feedback_answer));
            strcpy(feedback_answer, pWeather->valuestring);
            Log::info("------weatherdata=%s\n", feedback_answer);
            strcat(feedback_answer, pTemp->valuestring);
            Log::info("------weatherdata=%s\n", feedback_answer);
            strcat(feedback_answer, pWind->valuestring);
            Log::info("------weatherdata=%s\n", feedback_answer);
            if(NULL == pWeather)
            {
                Log::info("---no weather---\n");
                return Constant::kJsonQuestionNotFound;;
            }
            return feedback_answer;
            //result = pWeather->valuestring;
        }
        else if(!strcmp(service, "music"))
        {
            Log::info("------enter music -----\n");
            cJSON *pAnswer = cJSON_GetObjectItem(pJson, "data");
            if(NULL == pAnswer)
            {
                Log::info("---no data--\n");
                return Constant::kJsonQuestionNotFound;;
            }
            cJSON *pResult = cJSON_GetObjectItem(pAnswer, "result");
            if(NULL == pResult)
            {
                Log::info("----no result---\n");
                return Constant::kJsonQuestionNotFound;;
            }
            cJSON *pArray = cJSON_GetArrayItem(pResult, 0);
            if(NULL == pArray)
            {
                Log::info("----no array---\n");
                return Constant::kJsonQuestionNotFound;;
            }
            cJSON *pMusic = cJSON_GetObjectItem(pArray, "downloadUrl");
            if(NULL == pMusic)
            {
                Log::info("---no music---\n");
                return Constant::kJsonQuestionNotFound;;
            }
            sAnswer =  pMusic->valuestring;
            Log::info("---pMusic = %s--\n", pMusic->valuestring);
        }
        else if(!strcmp(service, "cookbook"))
        {
            cJSON *pAnswer = cJSON_GetObjectItem(pJson, "data");
            if(NULL == pAnswer)
            {
                Log::info("---no data--\n");
                return Constant::kJsonQuestionNotFound;;
            }
            cJSON *pResult = cJSON_GetObjectItem(pAnswer, "result");
            if(NULL == pResult)
            {
                Log::info("----no result---\n");
                return Constant::kJsonQuestionNotFound;;
            }
            cJSON *pArray = cJSON_GetArrayItem(pResult, 0);
            if(NULL == pArray)
            {
                Log::info("----no array---\n");
                return Constant::kJsonQuestionNotFound;;
            }
            cJSON *pCookbook = cJSON_GetObjectItem(pArray, "accessory");
            if(NULL == pCookbook)
            {
                Log::info("---no cookbook---\n");
                return Constant::kJsonQuestionNotFound;;
            }
            sAnswer =  pCookbook->valuestring;

        }
        else
        {
            cJSON *pAnswer = cJSON_GetObjectItem(pJson, "answer");
            if(NULL == pAnswer)
            {
                return Constant::kJsonQuestionNotFound;;
            }
            cJSON *pAnswer_text = cJSON_GetObjectItem(pAnswer, "text");
            if(NULL == pAnswer_text)
            {
                return Constant::kJsonQuestionNotFound;;
            }
            sAnswer = pAnswer_text->valuestring;
        }
    }
    else
    {
        return Constant::kJsonQuestionNotFound;;
    }
    return sAnswer;
}

/**
 * Semantic parsing
 *
 * @brief MSP_Process
 * @param src_text
 * @return
 */
void MSP_Process(const char* iatResult, void *args)
{
    int ret = MSP_SUCCESS;
    const char* jsonStr = NULL;
    const char* sAnswer = NULL;
    unsigned int strLen =  0;
    CAEUserData *userData = (CAEUserData*)args;
    char * singerN = "歌手是";
    char * musicN = "歌曲是";
    Log::debug("---iatResult=%s\n", iatResult);
    const char * curFunc;

    strLen = strlen(iatResult);
    userData->mscInProcess = true;

#ifndef USE_CMCC
    jsonStr = MSPSearch("nlp_version=2.0", iatResult, &strLen, &ret);
    if(ret != MSP_SUCCESS)
    {
        Log::error("MSPSearch failed errCode:%d\n", ret);
        goto _exit;
    }

    Log::debug("Semantic parsing finished with JsonStr: %s\n", jsonStr);
    sAnswer = parseJsonStr(jsonStr);
    Log::debug("parse_Json_Str: %s\n", sAnswer);
#else
    sAnswer = cmcc_do_semantic(iatResult, strlen(iatResult), args);
    curFunc = cmcc_get_currentFunc(userData);
    Log::debug("sAnswer:%s\n", sAnswer);
    Log::debug("---cmcc_music_list->cmcc_playing_state=%d,curFunc=%s--\n", cmcc_music_list->cmcc_playing_state, curFunc);

    if(cmcc_music_list->cmcc_playing_state != CMCC_PLAYSTATE_NULL && !strcmp(curFunc, "music"))   //music playmode control
    {
        if(sAnswer && strstr(sAnswer, "cycle"))
        {
            cmcc_music_list->cmcc_playing_type = CMCC_PLAYMODE_SIGNLE;
            Log::debug("---cmcc_music_list->cmcc_playing_type=%d \n", cmcc_music_list->cmcc_playing_type);
            if(cmcc_music_list->cmcc_playing_state != CMCC_PLAYSTATE_STOPPED)
            {
                stop_music_playing(gst_player_url);//break last playlist loop;
                cmcc_waiting_playlist_break();
            }
			TTSProcess::play_feedback_pcm("好的，音乐切换到单曲循环模式",args);
            goto _PLAY_SINGLE;
        }
        else if(sAnswer && strstr(sAnswer, "order"))
        {
            cmcc_music_list->cmcc_playing_type = CMCC_PLAYMODE_PLAYLIST;
            Log::debug("---cmcc_music_list->cmcc_playing_type=%d \n", cmcc_music_list->cmcc_playing_type);
            if(cmcc_music_list->cmcc_playing_state != CMCC_PLAYSTATE_STOPPED)
            {
                stop_music_playing(gst_player_url);//break last playlist loop;
                cmcc_waiting_playlist_break();
            }
		TTSProcess::play_feedback_pcm("好的，音乐切换到顺序播放模式",args);
            goto _PLAY_LIST;
        }
        else if(sAnswer && strstr(sAnswer, "loop"))
        {
            cmcc_music_list->cmcc_playing_type = CMCC_PLAYMODE_LOOP;
            Log::debug("---cmcc_music_list->cmcc_playing_type=%d \n", cmcc_music_list->cmcc_playing_type);
            if(cmcc_music_list->cmcc_playing_state != CMCC_PLAYSTATE_STOPPED)
            {
                stop_music_playing(gst_player_url);//break last playlist loop;
                cmcc_waiting_playlist_break();
            }
		TTSProcess::play_feedback_pcm("好的，音乐切换到循环播放模式",args);
            goto _PLAY_LIST;
        }
        else if(sAnswer && strstr(sAnswer, "random"))
        {
            cmcc_music_list->cmcc_playing_type = CMCC_PLAYMODE_RANDOM;
            cmcc_music_list->cmcc_cur_music_num = rand() % cmcc_music_list->cmcc_list_music_num;
            Log::debug("---cmcc_music_list->cmcc_playing_type=%d \n", cmcc_music_list->cmcc_playing_type);
            if(cmcc_music_list->cmcc_playing_state != CMCC_PLAYSTATE_STOPPED)
            {
                stop_music_playing(gst_player_url);//break last playlist loop;
                cmcc_waiting_playlist_break();
            }
		 TTSProcess::play_feedback_pcm("好的，音乐切换到随机播放模式",args);
            goto _PLAY_LIST;
        }
    }
    else//music playmode control,feedback not music mode
    {
        if(sAnswer && (strstr(sAnswer, "cycle")||strstr(sAnswer, "order")||strstr(sAnswer, "loop")||strstr(sAnswer, "random")))
        {
            TTSProcess::play_feedback_pcm("只有在音乐模式下才能切换播放模式",args);
            goto _exit;
        }
    }
	
    if(sAnswer && (strstr(sAnswer, "replay") || strstr(sAnswer, "continue")))
    {
        if(!strcmp(curFunc, "music"))   //music mode
        {
            if(cmcc_music_list->cmcc_playing_state == CMCC_PLAYSTATE_STOPPED)
            {
                //now only can resume last played music,but can not play playlist
                /*resume_music_playing(gst_player_url);
                cmcc_music_list->cmcc_playing_state = CMCC_PLAYSTATE_PLAYED;
                Log::debug("---resume_music_playing----\n");
                goto _exit;
                */
                Log::debug("---resume_music_playing done----\n");
	        TTSProcess::play_feedback_pcm("好的，继续播放音乐",args);
                if(cmcc_music_list->cmcc_playing_type == CMCC_PLAYMODE_SIGNLE)
                {
                    goto _PLAY_SINGLE;
                }
                else
                {
                    goto _PLAY_LIST;
                }
            }
        }
        else if(!strcmp(curFunc, "readbook"))     //readbook mode
        {
            Log::debug("## readbook replay||conotinue\n");
            Log::debug("## cmcc_readbook_info->cmcc_playing_state:%d\n", cmcc_readbook_info->cmcc_playing_state);
            if(cmcc_readbook_info->cmcc_playing_state == CMCC_PLAYSTATE_PAUSED)
            {
		TTSProcess::play_feedback_pcm("好的，继续播放听书",args);
                resume_music_playing(gst_player_url);
                cmcc_readbook_info->cmcc_playing_state = CMCC_PLAYSTATE_PLAYED;
                Log::info("----resume_music_playing---\n");
                usleep(100 * 1000);
                goto _exit;
            }
        }
        TTSProcess::play_feedback_pcm("当前没有播放音乐或者听书，无法继续播放",args);
        goto _exit;
    }
    else if(sAnswer && strstr(sAnswer, "pause"))
    {
        if(!strcmp(curFunc, "music"))   //music mode
        {
            if(cmcc_music_list->cmcc_playing_state == CMCC_PLAYSTATE_PLAYED)
            {
                stop_music_playing(gst_player_url);
                cmcc_waiting_playlist_break();
                Log::debug("---stop_music_playing done----\n");
		TTSProcess::play_feedback_pcm("好的,音乐已经停止播放",args);
                goto _exit;
            }
        }
        else if(!strcmp(curFunc, "readbook"))     //readbook mode
        {
            Log::debug("## readbook stop play\n");
            Log::debug("## cmcc_readbook_info->cmcc_playing_state:%d\n", cmcc_readbook_info->cmcc_playing_state);
            if(cmcc_readbook_info->cmcc_playing_state == CMCC_PLAYSTATE_PLAYED)
            {
                pause_music_playing(gst_player_url);
                cmcc_readbook_info->cmcc_playing_state = CMCC_PLAYSTATE_PAUSED;
                Log::info("----pause_music_playing---\n");
                usleep(100 * 1000);
		TTSProcess::play_feedback_pcm("好的,听书已经暂停播放",args);
                goto _exit;
            }
        }
        TTSProcess::play_feedback_pcm("当前没有播放音乐或者听书，无法暂停播放",args);
        goto _exit;
    }
    else if(sAnswer && strstr(sAnswer, "next"))
    {
        if(!strcmp(curFunc, "music"))   //music mode
        {
            if(get_music_state(gst_player_url) == GST_STATE_PAUSED)
            {
                if(cmcc_music_list->cmcc_list_music_num <= 0)
                {
                    Log::debug("---not playlist,ignore next----\n");
                    goto _exit;
                }
                stop_music_playing(gst_player_url);//break last playlist loop;
                cmcc_music_list->cmcc_cur_music_num ++;
                if(cmcc_music_list->cmcc_cur_music_num >= cmcc_music_list->cmcc_list_music_num)
                {
                    cmcc_music_list->cmcc_cur_music_num = 0;
                }
                cmcc_waiting_playlist_break();
                Log::debug("---play next,ready to play---\n");
		TTSProcess::play_feedback_pcm("好的,播放下一首音乐",args);
                switch(cmcc_music_list->cmcc_playing_type)
                {
                    case CMCC_PLAYMODE_RANDOM:
                        cmcc_music_list->cmcc_cur_music_num = rand() % cmcc_music_list->cmcc_list_music_num;
                    case CMCC_PLAYMODE_PLAYLIST:
                    case CMCC_PLAYMODE_LOOP:
                        goto _PLAY_LIST;
                    case CMCC_PLAYMODE_SIGNLE:
                        goto _PLAY_SINGLE;
                }
            }
        }
        else if(!strcmp(curFunc, "readbook"))     //readbook mode
        {
            Log::debug("############ readbook next\n");
            goto _exit;
        }
        TTSProcess::play_feedback_pcm("当前没有播放音乐或者听书，无法播放下一首",args);
        goto _exit;
    }
    else if(sAnswer && (strstr(sAnswer, "past") || strstr(sAnswer, "previous")))
    {
        if(!strcmp(curFunc, "music"))   //music mode
        {
            if(get_music_state(gst_player_url) == GST_STATE_PAUSED)
            {
                if(cmcc_music_list->cmcc_list_music_num <= 0)
                {
                    Log::debug("---not playlist,ignore prev----\n");
                    goto _exit;
                }
                stop_music_playing(gst_player_url);//break last playlist loop;
                if(cmcc_music_list->cmcc_cur_music_num <= 0)
                {
                    cmcc_music_list->cmcc_cur_music_num =  cmcc_music_list->cmcc_list_music_num - 1;
                }
                else
                {
                    cmcc_music_list->cmcc_cur_music_num--;
                }
                cmcc_waiting_playlist_break();
                Log::debug("---play prev,ready to play---\n");
		 TTSProcess::play_feedback_pcm("好的,播放上一首音乐",args);
                switch(cmcc_music_list->cmcc_playing_type)
                {
                    case CMCC_PLAYMODE_RANDOM:
                        cmcc_music_list->cmcc_cur_music_num = rand() % cmcc_music_list->cmcc_list_music_num;
                    case CMCC_PLAYMODE_PLAYLIST:
                    case CMCC_PLAYMODE_LOOP:
                        goto _PLAY_LIST;
                    case CMCC_PLAYMODE_SIGNLE:
                        goto _PLAY_SINGLE;
                }
            }
        }
        else if(!strcmp(curFunc, "readbook"))     //readbook mode
        {
            Log::debug("############ readbook past\n");
            goto _exit;
        }
        TTSProcess::play_feedback_pcm("当前没有播放音乐或者听书，无法播放上一首",args);
        goto _exit;
    }
    else if(sAnswer && strstr(sAnswer, "volume_minus"))
    {
        int cur_vol = DeviceIo::getInstance()->getVolume();
        cur_vol -= 15;
        if(cur_vol < USER_VOL_MIN)
        {
            cur_vol = USER_VOL_MIN;
            Log::debug("---already Minimum volum--\n");
        }
        Log::debug("---set volume to %d--\n", cur_vol);
        DeviceIo::getInstance()->setVolume(cur_vol);
	TTSProcess::play_feedback_pcm("好的,音量调小",args);
        resume_music_playing(gst_player_url);//resume playing
        goto _exit;

    }
    else if(sAnswer && strstr(sAnswer, "volume_plus"))
    {
        int cur_vol = DeviceIo::getInstance()->getVolume();
        cur_vol += 15;
        if(cur_vol > USER_VOL_MAX)
        {
            cur_vol = USER_VOL_MAX;
            Log::debug("---already maximum volume--\n");
        }
        Log::debug("---set volume to %d--\n", cur_vol);
        DeviceIo::getInstance()->setVolume(cur_vol);
	 TTSProcess::play_feedback_pcm("好的,音量调大",args);
        resume_music_playing(gst_player_url);//resume playing
        goto _exit;
    }
    else if(sAnswer && strstr(sAnswer, "mute"))
    {
        Log::debug("---set mute--\n");
	 TTSProcess::play_feedback_pcm("好的,音箱静音",args);
        DeviceIo::getInstance()->setMute(true);
        resume_music_playing(gst_player_url);//resume playing
        goto _exit;
    }
    else if(sAnswer && strstr(sAnswer, "cancel"))     //numute
    {
        Log::debug("---unmute--\n");
        DeviceIo::getInstance()->setMute(false);
	TTSProcess::play_feedback_pcm("好的,取消静音",args);
        resume_music_playing(gst_player_url);//resume playing
        goto _exit;
    }

    if(sAnswer && strstr(sAnswer, "broadcastsinger"))
    {
        Log::debug("---broadcastsinger---\n");
        //sAnswer = cmcc_music_list->cmcc_music_info[cmcc_music_list->cmcc_cur_music_num].singerName;
        bzero(feedback_answer, sizeof(feedback_answer));
        strcat(feedback_answer, singerN);
        strcat(feedback_answer, cmcc_music_list->cmcc_music_info[cmcc_music_list->cmcc_cur_music_num].singerName);
        sAnswer = feedback_answer;
    }
    else if(sAnswer && strstr(sAnswer, "broadcast"))
    {
        Log::debug("---broadcast---\n");
        //sAnswer = cmcc_music_list->cmcc_music_info[cmcc_music_list->cmcc_cur_music_num].musicName;
        bzero(feedback_answer, sizeof(feedback_answer));
        strcat(feedback_answer, musicN);
        strcat(feedback_answer, sAnswer = cmcc_music_list->cmcc_music_info[cmcc_music_list->cmcc_cur_music_num].musicName);
        sAnswer = feedback_answer;
    }
#endif

    if(sAnswer != NULL && strstr(sAnswer, "http:") && strstr(sAnswer, ".mp3"))   //play music
    {
        Log::debug("---play music mode---\n");
        stop_music_playing(gst_player_url);//break last playlist loop;
#ifdef USE_CMCC
        if(cmcc_music_list->cmcc_playing_type == CMCC_PLAYMODE_PLAYLIST && !strcmp(curFunc, "music"))   //playlist
        {
            while(1)
            {
_PLAY_LIST:
                Log::debug("---play musiclist,cur id =%d cmcc_music_list->cmcc_playing_type=%d---\n",
                        cmcc_music_list->cmcc_cur_music_num, cmcc_music_list->cmcc_playing_type);
                userData->mscInProcess = false;//not block main thread
                cmcc_music_list->cmcc_playing_state = CMCC_PLAYSTATE_PLAYED;
                play_music_url(cmcc_music_list->cmcc_music_info[cmcc_music_list->cmcc_cur_music_num].listenUrl, true); //waiting for play completed

                if(get_music_state(gst_player_url) == GST_STATE_PAUSED)   // being pause by wakeup word
                {
                    Log::debug("---play list detected pause state,break---\n");
                    cmcc_music_list->cmcc_playing_state = CMCC_PLAYSTATE_PAUSED;
                    goto _exit;
                }
                if(get_music_state(gst_player_url) == GST_STATE_NULL)   // being stop
                {
                    Log::debug("---play list detected stop state,break---\n");
                    cmcc_music_list->cmcc_playing_state = CMCC_PLAYSTATE_STOPPED;
                    goto _exit;
                }
                switch(cmcc_music_list->cmcc_playing_type)
                {
                    case CMCC_PLAYMODE_RANDOM:
                        cmcc_music_list->cmcc_cur_music_num = rand() % cmcc_music_list->cmcc_list_music_num;
                        break;
                    case CMCC_PLAYMODE_PLAYLIST:
                        cmcc_music_list->cmcc_cur_music_num ++;
                        if(cmcc_music_list->cmcc_cur_music_num >= cmcc_music_list->cmcc_list_music_num)   //play finished
                        {
                            Log::debug("---play list play finished,break---\n");
                            cmcc_music_list->cmcc_playing_state = CMCC_PLAYSTATE_FINISHED;
                            goto _exit;
                        }

                        break;
                    case CMCC_PLAYMODE_LOOP:
                        cmcc_music_list->cmcc_cur_music_num ++;
                        if(cmcc_music_list->cmcc_cur_music_num >= cmcc_music_list->cmcc_list_music_num)   //play finished
                        {
                            Log::debug("---play loop ---\n");
                            cmcc_music_list->cmcc_cur_music_num = 0;
                        }
                        break;
                    case CMCC_PLAYMODE_SIGNLE:
                        goto _PLAY_SINGLE;
                }
            }
        }
        else if(cmcc_music_list->cmcc_playing_type == CMCC_PLAYMODE_SIGNLE  && !strcmp(curFunc, "music"))
        {
            //single mode
_PLAY_SINGLE:
            Log::debug("---play musicsingle,cur id =%d ---\n", cmcc_music_list->cmcc_cur_music_num);
            userData->mscInProcess = false;//not block main thread
            cmcc_music_list->cmcc_playing_state = CMCC_PLAYSTATE_PLAYED;
            play_music_url(cmcc_music_list->cmcc_music_info[cmcc_music_list->cmcc_cur_music_num].listenUrl, true); //waiting for play completed
            cmcc_music_list->cmcc_playing_state = CMCC_PLAYSTATE_STOPPED;
            goto _exit;
        }
        else if(!strcmp(curFunc, "readbook"))
        {
            //single mode
_PLAY_READBOOK:
            Log::debug("## _PLAY_READBOOK readbook,cur id =%d ---\n", cmcc_readbook_info->cur_chapterId);
            userData->mscInProcess = false;//not block main thread
            cmcc_readbook_info->cmcc_playing_state = CMCC_PLAYSTATE_PLAYED;
            play_music_url(cmcc_readbook_info->listenUrl, true); //waiting for play completed
            cmcc_music_list->cmcc_playing_state = CMCC_PLAYSTATE_STOPPED;
            goto _exit;
        }
        else
        {
            play_music_url(sAnswer, false);
            goto _exit;
        }
#else 
        play_music_url(sAnswer, false);
        goto _exit;
#endif
    }

    ret = TTSProcess::textToSpeech(sAnswer, args);//play tts
    if(!ret)
    {
        Log::error("TTSProcess::textToSpeech() failed, error code: %d.\n", ret);
        goto _exit;
    }
    if(get_music_state(gst_player_url) == GST_STATE_PAUSED)   //after tts ,if play music is paused,resume playing
    {
        Log::debug("---play music resume after tts---\n");
        usleep(100 * 1000);
        if(!userData->wakeUping)   //not in wakeup process
        {
            resume_music_playing(gst_player_url);
        }
    }

_exit:
    userData->mscInProcess = false;

}

void textToSpeech(const char* text, void *args)
{
    bool ret;
    CAEUserData *userData = (CAEUserData*)args;
    userData->mscInProcess = true;
    ret = TTSProcess::textToSpeech(text, args);
    if(!ret)
    {
        Log::error("TTSProcess::textToSpeech() failed, error code: %d.\n", ret);
        goto _exit;
    }

    if(get_music_state(gst_player_url) == GST_STATE_PAUSED)   //after tts ,if play music is paused,resume playing
    {
        usleep(100 * 1000);
        if(!userData->wakeUping)   //not in wakeup process
        {
            resume_music_playing(gst_player_url);
        }
    }
_exit:
    userData->mscInProcess = false;
}

bool AudioProcess::MSP_Login(const char *loginParams)
{
    int ret;

    ret = MSPLogin(NULL, NULL, loginParams);
    if(ret != MSP_SUCCESS)
    {
        Log::error("MSPLogin failed\n");
        MSPLogout();
        return false;
    }

    return true;
}

/**
 *  Padding audio data from the cae into the isr
 *  _ QISRAudioWrite()
 *
 * @brief AudioProcess::paddingIatAudio
 * @param audioData
 * @param audioLen
 * @param userData
 * @return isContinue
 */
bool AudioProcess::paddingIatAudio(const char* audioData, unsigned int audioLen,
                                   CAEUserData *userData)
{
    IatState iatState;
    bool isContinue = true;

    if(userData->newSession)
    {
        AudioProcess::iatSessionId = IatProcess::beginQISRSession(Constant::kIatSessionParams);
        if(AudioProcess::iatSessionId == NULL)
        {
            Log::error("IatProcess::beginQISRSession faied\n");
            return false;
        }
    }

    iatState = IatProcess::writeISRAudioData(AudioProcess::iatSessionId, audioData,
               audioLen, userData->newSession);

    //Log::debug("suspendFlag: %d\n", userData->suspendFlag);
    switch(iatState)
    {
        case IAT_STATE_ERROR:
            isContinue = false;
            Log::debug("=> IAT_STATE_ERROR \n");
            break;
        case IAT_STATE_VAD:
        {
            std::thread thread(textToSpeech, Constant::kMSCVadPrompt, (void*)userData);
            thread.detach();
            isContinue = false;
            break;
        }
        case IAT_STATE_COMPLETE:
        {
            Log::debug("IatResult available: %s\n", IatProcess::iatResult);
            // Semantic parsing process and stop transmitting data
            std::thread thread(MSP_Process, IatProcess::iatResult, (void*)userData);
            thread.detach();
            isContinue = false;
            break;
        }
        default:
            isContinue = true;
            break;
    }

    return isContinue;
}

const char* AudioProcess::getIatSessionId()
{
    return AudioProcess::iatSessionId;
}
