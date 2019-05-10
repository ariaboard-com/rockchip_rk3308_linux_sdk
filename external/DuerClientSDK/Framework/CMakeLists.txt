cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(Framework LANGUAGES CXX)

include_directories("${LoggerUtils_SOURCE_DIR}/include")

add_subdirectory("DeviceIo")
add_subdirectory("Timer")
add_subdirectory("SocketApp")
