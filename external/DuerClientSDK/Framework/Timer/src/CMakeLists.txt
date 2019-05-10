
add_library(Timer SHARED
        Timer.cpp)

target_include_directories(Timer PUBLIC
        "${Timer_SOURCE_DIR}/include"
        "${LoggerUtils_SOURCE_DIR}/include")

# install target
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
