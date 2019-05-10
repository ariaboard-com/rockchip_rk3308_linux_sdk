
add_library(DeviceIo SHARED
            linux/InfoLed.cpp
            linux/DeviceIo.cpp
            linux/bluetooth.cpp)

target_link_libraries(DeviceIo Timer wpa_client SocketApp)

if(BLUEZ5_UTILS)
	add_definitions(-DBLUEZ5_UTILS)
endif(BLUEZ5_UTILS)

target_include_directories(DeviceIo PUBLIC
        "${DeviceIo_SOURCE_DIR}/include"
        "${SocketApp_SOURCE_DIR}/include"
        "${LoggerUtils_SOURCE_DIR}/include")

# install target
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

