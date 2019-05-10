#ifndef _CMCC_H
#define _CMCC_H
#include <json-c/json.h>
#include "cae_lib.h"
//use cmcc or kedaxunfei
//#define USE_CMCC 1

static char receive_data[8192];
static char send_data[2048];
static char music_data[15360];//music data 15k
static char readbook_data[1024];//read book URL
static char feedback_answer[1024];//feadback answer



typedef enum cmcc_msgtype
{
    UPLOAD_MSG_login = 1,
    UPLOAD_MSG_heartBeat,
    UPLOAD_MSG_pushDevInfo,
    UPLOAD_MSG_semantic,
    UPLOAD_MSG_getImsInfo,
    UPLOAD_MSG_getImsPhoneList,
    UPLOAD_MSG_pushAiMessage,
    UPLOAD_MSG_getMdevUser,
    UPLOAD_MSG_getMusicInfoById,

    DOWNLOAD_MSG_pushMusics ,
    DOWNLOAD_MSG_pushControl,
    DOWNLOAD_MSG_syncImsInfo,
    DOWNLOAD_MSG_syncImsBindStatus,
    DOWNLOAD_MSG_syncImsPhoneList,
    DOWNLOAD_MSG_pushMdevUser,
    DOWNLOAD_MSG_getDevInfo,
};



struct CMCC_MUSIC_INFO
{
    char musicId[25];
    char musicName[100];
    char singerName[100];
    char picUrl[512];
    char listenUrl[512];
    char lrcUrl[512];
    char isCollection;
    char bmp[20];
    char isCpAuth;
};

#define CMCC_PLAYMODE_SIGNLE 1
#define CMCC_PLAYMODE_PLAYLIST 2
#define CMCC_PLAYMODE_RANDOM 3
#define CMCC_PLAYMODE_LOOP 4

#define CMCC_PLAYSTATE_NULL 0
#define CMCC_PLAYSTATE_PLAYED 1
#define CMCC_PLAYSTATE_PAUSED 2
#define CMCC_PLAYSTATE_STOPPED 3
#define CMCC_PLAYSTATE_FINISHED 4




struct CMCC_MUSIC_LIST
{
    struct CMCC_MUSIC_INFO cmcc_music_info[20];
    int cmcc_cur_music_num;
    int cmcc_list_music_num;
    int cmcc_playing_type;// 1  is signal, 2 is playlist
    int cmcc_playing_state; // 0 is not played, 1 playing ,2 paused,3, stopped
};

struct CMCC_READBOOK_INFO
{
    char book_name[100];
    char book_id[15];
    char cur_chapterName[256];
    char pre_chapterId[15];
    char cur_chapterId[15];
    char next_chapterId[15];
    char listenUrl[1024];
    char payUrl[1024];
    int cmcc_playing_state; // 0 is not played, 1 playing ,2 paused,3, stopped
};


typedef struct _CMCC_SEND_JSON_DATA
{
    char* msgType;
    char* did;
    char* devKey;
    struct json_object * data;
    char* devVersion;
    struct json_object * raw;
} cmcc_make_request_data_type;

typedef struct _CMCC_RECEIVE_JSON_DATA
{
    char* msgType;
    json_object *data;
    char* msg;
    char* recode;
} cmcc_receive_request_data_type;

static int m_socket_fd;

int cmcc_tcp_init();
int cmcc_tcp_deinit(); 
void cmcc_semantic_send(const char* data,int data_length);
void cmcc_heartBeat_send();
const char* cmcc_semantic_receive(void* args);
void cmcc_heartBeat_receive();

void cmcc_do_heartBeat();
const char* cmcc_do_semantic(const char* data,int data_length,void* args);

void cmcc_ReadBook_send(const char* chapterId, const char* chapterQuality);
const char* cmcc_ReadBook_receive(cmcc_receive_request_data_type* cmcc_receive_request, json_object* receive_json_intent, void* args);

char*  cmcc_music_info_save(char* data, int playmode);
void cmcc_waiting_playlist_break();

void cmcc_set_currentFunc(CAEUserData *userData,char* func);
const char* cmcc_get_currentFunc(CAEUserData *userData);
extern struct CMCC_MUSIC_LIST *cmcc_music_list;
extern struct CMCC_READBOOK_INFO *cmcc_readbook_info;

void cmcc_testCommand(void* args, char* data, int delayExecTime);


#endif
