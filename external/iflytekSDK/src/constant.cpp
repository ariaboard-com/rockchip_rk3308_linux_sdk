#include "constant.h"

// The pcm file used for save audioData after wake_up
const char* Constant::kCaeOutPcmPath = "/userdata/cae_out.pcm";

// Cae_Library_Path for wake up _ cae_LoadLibrary()
const char* Constant::kCaeLibPATH = "/usr/lib/libcae.so";

// The word of wake up resource _ api_cae_new()
const char* Constant::kIwvResPath = "/etc/ivw_resource.jet";

// msc login params _ MSPLogin()
// don't change it
const char* Constant::kMSCLoginParams = "appid = 58bcdd98, work_dir = .";

const char* Constant::kIatSessionParams =
        "sub = iat, domain = iat, language = zh_cn, "
        "accent = mandarin, sample_rate = 16000, "
        "result_type = plain, result_encoding = utf8";

const char* Constant::kTTSsessionParams =
        "voice_name = xiaoyan, text_encoding = utf8, sample_rate = 16000,"
        "speed = 50, volume = 50, pitch = 50, rdn = 2";

const char* Constant::kJsonQuestionNotFound = "未找到问题答案";
const char* Constant::kMSCVadPrompt = "没听到声音";

const char* Constant::kCaeWakeupWorkds = "file:///oem/wozai.wav";
const char* Constant::kWifiConnected = "file:///usr/ap_notification/wifi_conneted.mp3";

const char* Constant::kJsonNotLink = "未找到播放链接";
const char* Constant::kJsonNotPreChapter = "没有找到上一章";
const char* Constant::kJsonNotNextChapter = "没有找到下一章";
