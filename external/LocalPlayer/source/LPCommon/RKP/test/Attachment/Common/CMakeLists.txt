add_library(AttachmentCommonTestLib
        Common.cpp)
target_include_directories(AttachmentCommonTestLib PUBLIC
        "${LPCommon_INCLUDE_DIRS}")
target_link_libraries(AttachmentCommonTestLib
        LPCommon
        gtest_main
        gmock_main)
