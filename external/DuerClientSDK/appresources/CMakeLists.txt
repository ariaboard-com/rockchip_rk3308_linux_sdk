cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(APPRESOURCES LANGUAGES CXX)

include(../build/BuildDefaults.cmake)
message("${PROJECT_SOURCE_DIR}/${Platform}/")
if(EXISTS "${PROJECT_SOURCE_DIR}/${Platform}/")
    install(DIRECTORY
            "${PROJECT_SOURCE_DIR}/${Platform}/"
            DESTINATION
            "${CMAKE_INSTALL_PREFIX}/appresources"
            PATTERN "app_config.json*" EXCLUDE)
    if(DEBUG_FLAG)
        install(FILES "${PROJECT_SOURCE_DIR}/${Platform}/app_config.json.debug"
                DESTINATION "${CMAKE_INSTALL_PREFIX}/appresources"
                RENAME "app_config.json")
    else()
        install(FILES "${PROJECT_SOURCE_DIR}/${Platform}/app_config.json.release"
                DESTINATION "${CMAKE_INSTALL_PREFIX}/appresources"
                RENAME "app_config.json")
    endif()
else()
    install(DIRECTORY
            "${PROJECT_SOURCE_DIR}/General/"
            DESTINATION
            "${CMAKE_INSTALL_PREFIX}/appresources")
endif()

