
add_library(SocketApp SHARED
        socket_app.cpp)

target_include_directories(SocketApp PUBLIC
        "${SocketApp_SOURCE_DIR}/include"
        "${LoggerUtils_SOURCE_DIR}/include")

# install target
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
