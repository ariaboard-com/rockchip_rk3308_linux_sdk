project(LPCommon)

find_package(CURL ${CURL_PACKAGE_CONFIG})

add_definitions("-DACSDK_LOG_MODULE=rkpCommon")

add_subdirectory("RKP")
add_subdirectory("Utils")

add_library(LPCommon SHARED
    RKP/src/RKPDirective.cpp
    RKP/src/RKPMessage.cpp
    RKP/src/RKPMessageHeader.cpp
    RKP/src/AbstractConnection.cpp
    RKP/src/LocalPlayerSDKInit.cpp
    RKP/src/Attachment/Attachment.cpp
    RKP/src/Attachment/AttachmentManager.cpp
    RKP/src/Attachment/InProcessAttachment.cpp
    RKP/src/Attachment/InProcessAttachmentReader.cpp
    RKP/src/Attachment/InProcessAttachmentWriter.cpp
    RKP/src/CapabilityAgent.cpp
    RKP/src/DialogUXStateAggregator.cpp
    RKP/src/EventBuilder.cpp
    RKP/src/ExceptionEncounteredSender.cpp
    RKP/src/HandlerAndPolicy.cpp
    RKP/src/MessageRequest.cpp
    RKP/src/NamespaceAndName.cpp
    Utils/src/Configuration/ConfigurationNode.cpp
    Utils/src/Executor.cpp
    Utils/src/FileUtils.cpp
    Utils/src/JSONUtils.cpp
    Utils/src/LibcurlUtils/CurlEasyHandleWrapper.cpp
    Utils/src/LibcurlUtils/CurlMultiHandleWrapper.cpp
    Utils/src/LibcurlUtils/HTTPContentFetcherFactory.cpp
    Utils/src/LibcurlUtils/HttpPost.cpp
    Utils/src/LibcurlUtils/LibCurlHttpContentFetcher.cpp
    Utils/src/LibcurlUtils/LibcurlUtils.cpp
    Utils/src/Logger/ConsoleLogger.cpp
    Utils/src/Logger/Level.cpp
    Utils/src/Logger/LogEntry.cpp
    Utils/src/Logger/LogEntryBuffer.cpp
    Utils/src/Logger/LogEntryStream.cpp
    Utils/src/Logger/Logger.cpp
    Utils/src/Logger/LoggerSinkManager.cpp
    Utils/src/Logger/LoggerUtils.cpp
    Utils/src/Logger/ModuleLogger.cpp
    Utils/src/Logger/ThreadMoniker.cpp
    Utils/src/Metrics.cpp
    Utils/src/RequiresShutdown.cpp
    Utils/src/RetryTimer.cpp
    Utils/src/Stream/StreamFunctions.cpp
    Utils/src/Stream/Streambuf.cpp
    Utils/src/StringUtils.cpp
    Utils/src/TaskQueue.cpp
    Utils/src/TaskThread.cpp
    Utils/src/TimePoint.cpp
    Utils/src/TimeUtils.cpp
    Utils/src/Timer.cpp
    Utils/src/UUIDGeneration.cpp)

target_include_directories(LPCommon PUBLIC
    "${LPCommon_SOURCE_DIR}/RKP/include"
    "${LPCommon_SOURCE_DIR}/SDKInterfaces/include"
    "${LPCommon_SOURCE_DIR}/Utils/include"
    "${RAPIDJSON_INCLUDE_DIR}"
    ${CURL_INCLUDE_DIRS})

target_link_libraries(LPCommon
    ${CURL_LIBRARIES})

# install target
LIST(APPEND PATHS "${PROJECT_SOURCE_DIR}/RKP/include")
LIST(APPEND PATHS "${PROJECT_SOURCE_DIR}/SDKInterfaces/include")
LIST(APPEND PATHS "${PROJECT_SOURCE_DIR}/Utils/include")
asdk_install_multiple("${PATHS}")
