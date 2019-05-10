cmake_minimum_required(VERSION 2.6.2)

project(log CXX C)

option(TARGET_USES_LOGD "Use logd or not." OFF)

if(${TARGET_USES_LOGD})
	set(LIBLOG_SRCS logd_write.c log_event_write.c log_read.c)
else()
	set(LIBLOG_SRCS logd_write_kern.c log_read_kern.c)
endif()

set(LIBLOG_SRCS ${LIBLOG_SRCS} logprint.c event_tag_map.c)
set(LIBLOG_SRCS ${LIBLOG_SRCS} fake_log_device.c)

add_definitions(-DFAKE_LOG_DEVICE=1 -DHAVE_SYS_UIO_H=1 -DHAVE_WRITEV)

include_directories(${log_SOURCE_DIR}/include)

add_library(log
	STATIC
	${LIBLOG_SRCS}
	)

# If the CMake version supports it, attach header directory information
# to the targets for when we are part of a parent build (ie being pulled
# in via add_subdirectory() rather than being a standalone build).
if (DEFINED CMAKE_VERSION AND NOT "${CMAKE_VERSION}" VERSION_LESS "2.8.11")
	target_include_directories(log      INTERFACE "${log_SOURCE_DIR}/include")
endif()

install(TARGETS log ARCHIVE DESTINATION lib)

install(DIRECTORY ${log_SOURCE_DIR}/include/android DESTINATION include)
install(DIRECTORY ${log_SOURCE_DIR}/include/log DESTINATION include)
