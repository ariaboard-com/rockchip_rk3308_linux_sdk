file(GLOB_RECURSE Server_SRC "${softapServer_SOURCE_DIR}/src/*.cpp")

add_executable(softapServer ${Server_SRC})
install(TARGETS softapServer DESTINATION bin)

target_include_directories(softapServer PUBLIC 
	"${softapServer_SOURCE_DIR}/include"
	"${CMAKE_THREAD_LIBS_INIT}"
	${RAPIDJSON_INCLUDE_DIR})

target_link_libraries(softapServer pthread) 
