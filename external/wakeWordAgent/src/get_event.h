#ifdef __cplusplus
extern "C" {
#endif
#define MAX_KEY_BUFFERED	8

struct alexa_key {
	int key_code;
	int is_long_press;
	int is_key_valid;
	/*
	 * pre_alexa_mode is used if only one key to switch from different mode, such as wifi/bt/mic_mute mode.
	 * we can define the previous alexa mode as ALEXA_WIFI_MODE if we want to press one key to switch from 
	 * wifi mode to other mode.
	 * if pre_alexa_mode defined as ALEXA_INVALID_MODE, means this value is not used
	*/
//	int pre_alexa_mode;
	int (*key_process)(void);
	struct timeval time;
};
enum alexa_mode {
	ALEXA_NORMAL_MODE = 0,
	ALEXA_WIFI_MODE,
	ALEXA_BT_MODE,
	ALEXA_INVALID_MODE,
};
enum event_type {
	FUNC_KEY_WAKEUP  = 0,
	FUNC_KEY_WIFI_MODE,
	FUNC_KEY_VOL_DOWN,
	FUNC_KEY_VOL_UP,
	FUNC_KEY_MIC_MUTE,
	FUNC_KEY_BT_MODE,
	FUNC_KEY_NORMAL_MODE,
	FUNC_KEY_MIC_UNMUTE,
	FUNC_VOLUME_CHANGE,
	FUNC_LAST_ID,
};

enum support_event_type {
	EVENT_START = 0,
	KEY_EVENT = EVENT_START, //this is the first type of event
	ROTARY_EVENT,
	EVENT_END = ROTARY_EVENT,
};

struct alexa_support_event_type {
	int		event_type;
	char		*name; /*event name, used to judge this event is supported or not*/
};

struct alexa_vol_control {
	int		vol_step;
	int		alex_vol;
	int		codec_vol;
};

int start_event_thread(void);
int event_process(void);
void alexa_volume_set_step(int vol);
int alexa_volume_get_step(void);
void alexa_volume_set_mute();
int codec_set_vol(int vol_step);
enum alexa_mode alexa_get_mode();
void alexa_set_mode(enum alexa_mode mode);

#ifdef __cplusplus
} /* extern "C" */
#endif /* C++ */
