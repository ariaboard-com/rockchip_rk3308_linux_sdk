if(${BUILD_USE_RECORDER} MATCHES "PORTAUDIO")
project(PortAudioMicrophoneWrapper LANGUAGES CXX)

add_library(PortAudioMicrophoneWrapper
        SHARED
        PortAudioMicrophoneWrapper.cpp)

target_include_directories(PortAudioMicrophoneWrapper PUBLIC
        "${DCSApp_SOURCE_DIR}/include"
        "${DeviceIo_SOURCE_DIR}/include"
        "${DeviceTools_SOURCE_DIR}/include"
        "${LoggerUtils_SOURCE_DIR}/include"
        "${DcsSdk_SOURCE_DIR}/include"
        )

target_link_libraries(PortAudioMicrophoneWrapper PUBLIC
        "${PORTAUDIO_LIB_PATH}")

install(TARGETS ${PROJECT_NAME} DESTINATION "${ASDK_LIB_INSTALL_DIR}")
endif()

if(${BUILD_USE_RECORDER} MATCHES "ALSA")
project(AlsaAudioMicrophoneWrapper LANGUAGES CXX)

add_library(AlsaAudioMicrophoneWrapper
        SHARED
        AlsaAudioMicrophoneWrapper.cpp
        AlsaRecorder.cpp)

target_include_directories(AlsaAudioMicrophoneWrapper PUBLIC
        "${DCSApp_SOURCE_DIR}/include"
        "${DeviceIo_SOURCE_DIR}/include"
        "${DeviceTools_SOURCE_DIR}/include"
        "${LoggerUtils_SOURCE_DIR}/include"
        "${DcsSdk_SOURCE_DIR}/include"
        )
target_link_libraries(AlsaAudioMicrophoneWrapper PUBLIC
        asound)
install(TARGETS ${PROJECT_NAME} DESTINATION "${ASDK_LIB_INSTALL_DIR}")
endif()






