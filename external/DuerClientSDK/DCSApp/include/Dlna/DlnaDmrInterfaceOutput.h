//
// Created by eddy on 18-2-3.
//

#ifndef DUEROS_DCS_APP_DLNA_DLNADMRINTERFACEOUTPUT_H
#define DUEROS_DCS_APP_DLNA_DLNADMRINTERFACEOUTPUT_H

#include <string>

namespace duerOSDcsApp {
namespace dueros_dlna {

enum PlayFeedback {
    PLAY_STOPPED,
    PLAY_PLAYING,
    PLAY_PAUSED_PLAYBACK,
    PLAY_STARTED_NEXT_STREAM,
    PLAY_NO_MEDIA_PRESENT
};

typedef void (* output_transition_cb_t)(enum PlayFeedback);

typedef void (* output_update_meta_cb_t)(const struct SongMetaData*);

class IOutput {
public:
    /**
     * 命令解析
     * @param ctx:glib命令行解析上下文
     * @return
     */
    //    virtual int add_options(GOptionContext* ctx)=0;

    /**
     * 播放器初始化
     * @return
     */
    virtual int init(void) = 0;

    /**
     *待播放媒体播放资源地址设置
     * @param uri:媒体资源地址
     * @param meta_info：媒体信息回调
     */
    virtual void set_uri(const std::string uri, output_update_meta_cb_t meta_info) = 0;

    /**
     *下一个待播放媒体播放资源地址设置
     * @param uri：下一个需要播放的媒体资源地址
     */
    virtual void set_next_uri(const std::string uri) = 0;

    /**
     *播放
     * @param transition_callback：播放状态回调
     * @return
     */
    virtual int play(output_transition_cb_t transition_callback) = 0;

    /**
     *停止
     * @return
     */
    virtual int stop(void) = 0;

    /**
     *暂停
     * @return
     */
    virtual int pause(void) = 0;

    /**
     *seek
     * @param position_nanos：seek位置
     * @return
     */
    virtual int seek(long position_nanos) = 0;

    /**
     *获取当前播放位置
     * @param track_duration
     * @param track_pos
     * @return
     */
    virtual int get_position(long* track_duration, long* track_pos) = 0;

    /**
     * 获取当前音量
     * @param val ：当前音量值
     * @return
     */
    virtual int get_volume(float* val) = 0;

    /**
     * 设置当前音量
     * @param val:音量值
     * @return
     */
    virtual int set_volume(float val) = 0;

    /**
     * 获取mute的设置值
     * @param val ：mute设置值
     * @return
     */
    virtual int get_mute(int* val) = 0;

    /**
     * 静音
     * @param val：静音值操作
     * @return
     */
    virtual int set_mute(int val) = 0;

    virtual int get_state(void) = 0;

    virtual int resume(void) = 0;

};
}
}

#endif //DLNA_DMR_BASE_OUTPUT_H
