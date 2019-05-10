# 基于Linux版DCS SDK构建智能语音应用

## DCS协议

DCS协议是DuerOS服务端与设备端之间的通讯协议，是一套把DuerOS的智能语音交互能力向所有设备开放的API。智能硬件通过这套API与服务端进行通讯，实现DCS协议客户端逻辑，接入DuerOS服务，就能够让设备具有DuerOS的所有交互能力，了解详情请看[DCS协议文档](http://developer.dueros.baidu.com/doc/dueros-conversational-service/introduction_markdown)。

## 摘要

Linux版DCS SDK基于DCS协议，提供给开发者一个在智能设备与DuerOS之间进行智能语音交互实现的开发包，旨在降低设备接入DuerOS的开发成本。开发包包括构建智能应用与DuerOS交互时所需要的相关的头文件、SO库、开发文档、一个完整的Sample Application代码，以及相关编译脚本，开发者可通过在编译脚本中配置自己对应的编译器环境，一键编译出适配自己平台的智能应用，并在相应目标平台中体验DuerOS的交互流程。

## 1、前置条件

### 1）在DuerOS开发者平台完成百度账号注册、开发者认证和产品创建流程（选择Linux平台），并保存对应产品的client_id

![图片](http://bos.nj.bpc.baidu.com/v1/agroup/0e6b3fbdf42af6796c0652db165dc7b2d4fc1e46)

具体参考[控制台接入流程](http://developer.dueros.baidu.com/doc/overall/console-guide_markdown)。

### 2）打开工程文件resources/config.json中，并替换clientId为对应产品client id（需要配合小度之家SDK进行帐号的授权操作）

![图片](http://bos.nj.bpc.baidu.com/v1/agroup/0e74f45b31763364a2227e4f62af809d7645fc49)

### 3）开发环境需求
  - Ubuntu 16.04或以上64位版本
  - 编译工具链需支持C++11（建议使用GNU GCC 4.9.4以上版本）
  - Cmake 3.6.3或以上版本

### 4）第三方开源库依赖
Linux DCS SDK依赖如下开源库：
  - openssl: 1.0.2g
  - zlib: 1.2.8
  - nghttp2: 1.24.0
  - libcurl: 7.54.1
  - sqlite3: 3200100
  - iconv: 1.14

基于Linux DCS SDK实现的Sample Application依赖如下开源库：
  - portaudio: 190600——20161030
  - ffmpeg: 3.2.7
## 2、工程介绍
![图片](http://bos.nj.bpc.baidu.com/v1/agroup/21dcf534094abf63541a6a26867f34be259710fe)  
| 目录&&文件                         | 说明               |  
| ------------------------------ | ---------------- |  
| appresources                   | SampleApp资源目录    |  
| build                          | 相关交叉编译工具链配置      |  
| build.sh                       | 编译脚本             |  
| DCSApp                         | SampleApp源代码目录   |  
| resources                      | DCS SDK资源目录      |  
| common.res                     | 唤醒资源文件           |  
| config.json                    | DCS SDK配置文件      |  
| xiaoduxiaodu_all_10022017.umdl | 唤醒模型文件           |  
| sdk                            | DCS SDK相关头文件和so库 |  
| third                          | DCS SDK第三方依赖库    |  


## 3、Sample Application程序说明

### 1）找到SampleApp程序入口文件main.cpp

    //创建一个DCSApplication实例
    auto dcsApplication = duerOSDcsApp::application::DCSApplication::create();
    
    //通过调用run函数让DCSApplication能持续运行直到退出
    dcsApplication->run();


### 2）DCSApplication中进行DCS SDK的初始化和调用
在initialize()函数中进行DCS SDK的初始化和调用。

创建相关的播放器实例：

    //读取SampleApp配置文件 
    Configuration::getInstance()->readConfig()
        
    //创建语音播报播放器实例，传入参数为音频设备节点
    auto speakMediaPlayer = mediaPlayer::TtsPlayerProxy::create(configuration->getTtsPlaybackDevice());
        
    //创建音乐播放播放器实例，传入参数为音频设备节点
    auto audioMediaPlayer = mediaPlayer::MediaPlayerProxy::create(configuration->getMusicPlaybackDevice());
        
    //创建闹钟闹铃播放器实例，传入参数为音频设备节点
    auto alertsMediaPlayer = mediaPlayer::AlertsPlayerProxy::create(configuration->getAlertPlaybackDevice());
        
    //创建提示音播放器实例，传入参数为音频设备节点
    auto localMediaPlayer = mediaPlayer::LocalPlayerProxy::create(configuration->getInfoPlaybackDevice());
        
    //创建本地语音合成播放器节点，用于将文字离线转换为语音播报，传入参数为音频设备节点
    auto localTtsPlayer = mediaPlayer::LocalTtsProxy::create(configuration->getNattsPlaybackDevice());
        
    //创建蓝牙播放器，如果硬件设备支持并需要蓝牙播放功能可以创建此实例
    auto blueToothPlayer = mediaPlayer::BlueToothPlayerProxy::create();

初始化DCS SDK:

    //ApplicationManager实现了DCS SDK的相关接口(包括DialogUXStateObserverInterface、ConnectionStatusObserverInterface、ApplicationImplementation)
     auto applicationManager = std::make_shared<ApplicationManager>();
            
    /// SDK初始化所需要的参数
    duerOSDcsSDK::sdkInterfaces::DcsSdkParameters parameters;
    /// SDK配置文件所在的目录
    parameters.setPathToConfig(PATH_TO_CONFIG);
    /// SDK运行时动态写入的配置文件路径（启动程序时文件可不存在，但路径必须有效可写）
    parameters.setPathToRuntimeConfig(PATH_TO_RUNTIME_CONFIG);
    /// 设备的唯一编号
    parameters.setDeviceId();
    /// TTS的播放器实例
    parameters.setSpeakMediaPlayer(speakMediaPlayer);
    /// 音频播放器实例
    parameters.setAudioMediaPlayer(audioMediaPlayer);
    /// 闹钟播放器实例
    parameters.setAlertsMediaPlayer(alertsMediaPlayer);
    /// 本地提示音播放器
    parameters.setLocalAlarmMediaPlayer(localMediaPlayer);
    /// 会话状态观察类实例
    parameters.setDialogStateObservers({applicationManager});
    /// 连接状态观察类实例
    parameters.setConnectionObservers(applicationManager);
    /// 一些设备相关操作的接口实现类实例
    parameters.setApplicationImplementation(applicationManager);
    /// 蓝牙资源播放器实例
    parameters.setLocalMediaPlayer(blueToothPlayer);
    /// 是否使用SDK内置的唤醒库
    parameters.setEnableSdkWakeup(true);
    
    auto voiceAndTouchWakeUpObserver = std::make_shared<VoiceAndTouchWakeUpObserver>();
    parameters.setKeyWordObserverInterface(voiceAndTouchWakeUpObserver);   
    #ifdef LOCALTTS
    parameters.setLocalTtsMediaPlayer(localTtsPlayer);
    #endif
    m_dcsSdk = duerOSDcsSDK::sdkInterfaces::DcsSdk::create(parameters);
        
    if (!m_dcsSdk) {
        bduer::Logger::log_error("Failed to create default SDK handler!");
        return false;
    }

初始化PortAudio录音工具:

    //创建PortAudio录音包装器实例
    std::shared_ptr<PortAudioMicrophoneWrapper> micWrapper = PortAudioMicrophoneWrapper::create(m_dcsSdk);

启用DuerLink相关功能:

    //初始化DuerLink相关实例
    DuerLinkWrapper::getInstance()->initDuerLink();
    //启用配网相关功能
    DuerLinkWrapper::getInstance()->startNetworkRecovery();
    //启用设备发现和DLP(需要配合小度之家SDK进行使用)相关功能
    DuerLinkWrapper::getInstance()->startDiscoverAndBound(m_dcsSdk->getClientId());


## 4、Sample Application使用说明

### 1）编译
  
在工程下运行  
  
$sh build.sh build/build.conf  
  
可以在工程下生成xxx_buildout目录  

### 2）运行
  
进入到xxx_buildout/output目录下，运行  
  
$LD_LIBRARY_PATH=./lib ./duer_linux  
  
## 5、唤醒

说“小度小度”，成功唤醒后进入语音识别状态，此时就可以进行相关的语音交互。

## 6、和小度对话

请尝试多种Query，与小度交互

- 北京天气怎么样
- 播放刘若英的歌
- 播放鬼吹灯
- 播放长春广播
- 中国最高的山
- 500乘以500加30等于多少

## 7、FAQ
##### Linux版DCS SDK对智能语音设备的最低硬件配置需求是怎样的？
    单核CPU 1GHz及以上
    运行Flash 128M及以上
    运行内存 64M及以上
    编译链支持C++11（建议GNU GCC 4.9.4以上版本）

##### 如果不使用默认的PortAudio库采集音频，使用其他方式采集音频，该如何实现？
    参照PortAudioMicrophoneWrapper的实现，在采集到音频数据时，通过调用DcsSdk的writeAudioData函数，向DuerOS传送音频数据。

##### 如何不通过语音唤醒的方式与DuerOS交互？
    首先在调用DcsSdk的create函数时，禁用SDK自有唤醒功能：parametrs.setEnableSdkWakeup(false)；
    其次在触发与DuerOS的每一次交互前，调用DcsSdk的notifyOfTapToTalk函数通知DuerOS开始语音识别。

##### Linux版DCS SDK目前支持哪些Skill？
    目前Linux版本DCS SDK支持语音唤醒、语音识别、闹钟功能、有声资源播放、音乐资源播放、设备音量控制、播放控制、TTS播放等功能，开发者可在Sample Application中结合智能应用运行状态、DuerOS状态实现自定义的特色功能。

##### 智能设备如何使用自己定制的bot能力？
    开发者在DuerOS开放平台注册自定义的指令集合后，可通过在DcsSdk的create函数中传递自定义的指令集集合，在向DuerOS发出特定的语音Query后，ApplicationImplementation会接收到Query对应的自定义指令，开发者在informCustomizeDirective中解析DuerOS返回的自定义指令，并对应到自己的端能力，即可实现个性化bot能力。

##### 账号登录失败，日志中出现Peer certificate cannot be authenticated with given CA certificates错误信息，该如何解决？
    Linux版DCS SDK通过HTTPS访问DuerOS服务，并需要对应的证书信息，默认证书信息在/etc/ssl/certs/目录下，用户也可以通过在resources/config.json中修改curloptCapath配置指定证书目录，另外设备也需要有同步最新时间的能力，否则证书验证也会失败。

##### 闹钟功能不能用？
    需要使用百度账号登录，在小度之家配置后才能使用。

##### Sample Application调用哪个接口进入配网模式？
    DuerLinkWrapper->startNetworkConfig()  

##### Sample Application代码太多，如何快速上手？
    Sample Application通过调用Linux版DCS SDK提供的API，并实现对应的DuerOS播放器接口、观察类接口、设备操控接口来实现智能设备的个性化应用，可以分解为如下模块来熟悉代码：
    - DcsSdk API接口模块
    - 媒体播放器观察类接口以及对应应用层实现模块（包括音乐播放器、TTS播放器、蓝牙播放器、闹钟提示音播放器等）
    - 应用层接口模块（包括设备信息获取、设备操控等功能）
    - 配网，账号登录模块
    - 录音，唤醒模块
    - 自定义定制的bot指令调用
