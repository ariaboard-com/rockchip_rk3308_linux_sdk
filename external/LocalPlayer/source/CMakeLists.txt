cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

# Set project information
project(LocalPlayerSDK VERSION 1.3.0 LANGUAGES CXX)
set(PROJECT_BRIEF "A cross-platform, modular SDK for interacting with the Local Player")

include(build/BuildDefaults.cmake)
include(tools/Testing.cmake)

# Set variables for target install and .pc pkg-config file
include(build/cmake/PrepareInstall.cmake)

# Local Player SDK targets.
add_subdirectory("ThirdParty")
add_subdirectory("LPCommon")
add_subdirectory("MediaPlayer")
add_subdirectory("PlaylistParser")
# Create .pc pkg-config file
include(build/cmake/GeneratePkgConfig.cmake)
