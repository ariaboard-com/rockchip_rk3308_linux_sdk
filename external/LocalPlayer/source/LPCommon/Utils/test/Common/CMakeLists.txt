add_library(UtilsCommonTestLib MockMediaPlayer.cpp)
target_include_directories(UtilsCommonTestLib PUBLIC
        "${LPCommon_INCLUDE_DIRS}"
	"${LPCommon_SOURCE_DIR}/Utils/test")
target_link_libraries(UtilsCommonTestLib
        LPCommon
        gtest_main
        gmock_main)
