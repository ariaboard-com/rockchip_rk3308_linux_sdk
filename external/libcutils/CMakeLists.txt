project(libcutils)

enable_language(ASM)

add_definitions(-DANDROID_SMP=1 -DHAVE_PTHREADS -DHAVE_SYS_SOCKET_H -DHAVE_STRLCPY=0 -DHAVE_MEMSET32 -DHAVE_MEMSET16)

SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS ON)

MESSAGE(STATUS "CMAKE_SYSTEM_PROCESSOR:" ${CMAKE_SYSTEM_PROCESSOR})

IF(${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64") 
SET(memset "arch-arm64/android_memset.S")
ELSEIF(${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm") 
SET(memset "arch-arm/memset32.S")
ENDIF()

set(LIBCUTILS_SRC_FILES
    hashmap.c
    properties.c
    atomic.c
    native_handle.c
    socket_inaddr_any_server.c
    socket_local_client.c
    socket_local_server.c
    socket_loopback_client.c
    socket_loopback_server.c
    socket_network_client.c
    sockets.c
    strlcpy.c
    config_utils.c
    load_file.c
    open_memstream.c
    strdup16to8.c
    strdup8to16.c
    record_stream.c
    process_name.c
    threads.c
    sched_policy.c
    iosched_policy.c
    str_parms.c
    ashmem-dev.c
    debugger.c
    klog.c
    partition_utils.c
    qtaguid.c
    trace-dev.c
    uevent.c
    fs_config.c     
    ${memset}
)

add_library(cutils SHARED ${LIBCUTILS_SRC_FILES})
install(TARGETS cutils DESTINATION lib)
install(DIRECTORY cutils DESTINATION include)

target_include_directories(cutils
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
