add_executable(LocalPlayer LocalPlayer.cpp)

target_include_directories(LocalPlayer PUBLIC 
    "${MediaPlayer_SOURCE_DIR}/include"
    "${RAPIDJSON_INCLUDE_DIR}"
    "${GST_INCLUDE_DIRS}"
    "${MediaPlayer_SOURCE_DIR}/../PlaylistParser/include")

target_link_libraries(LocalPlayer MediaPlayer "${GST_LDFLAGS}" LPCommon PlaylistParser) 
