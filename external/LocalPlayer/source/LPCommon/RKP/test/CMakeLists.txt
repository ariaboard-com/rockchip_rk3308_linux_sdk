add_subdirectory("Attachment")
set(INCLUDE_PATH
    "${LPCommon_INCLUDE_DIRS}"
    "${LPCommon_SOURCE_DIR}/RKP/test"
    "${LPCommon_SOURCE_DIR}/SDKInterfaces/test")
discover_unit_tests("${INCLUDE_PATH}" "LPCommon;AttachmentCommonTestLib")
